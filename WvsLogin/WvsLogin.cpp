#include "WvsLogin.h"
#include "LoginEntry.h"
#include <thread>
#include <iostream>
#include <functional>
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "..\WvsLib\Common\ServerConstants.hpp"

WvsLogin::WvsLogin()
{
}

WvsLogin::~WvsLogin()
{
}

int WvsLogin::GetCenterCount() const
{
	return m_nCenterCount;
}

std::shared_ptr<Center>& WvsLogin::GetCenter(int idx)
{
	return m_apCenterInstance[idx];
}

void WvsLogin::SetConfigLoader(ConfigLoader * pCfg)
{
	m_pCfgLoader = pCfg;
}

void WvsLogin::ConnectToCenter(int nCenterIdx)
{
	m_apCenterInstance[nCenterIdx]->SetSocketDisconnectedCallBack(std::bind(&Center::OnNotifyCenterDisconnected, m_apCenterInstance[nCenterIdx].get()));
	m_apCenterInstance[nCenterIdx]->SetCenterIndex(nCenterIdx);
	m_apCenterInstance[nCenterIdx]->Connect(
		m_pCfgLoader->StrValue("Center" + std::to_string(nCenterIdx) + "_IP"),
		m_pCfgLoader->IntValue("Center" + std::to_string(nCenterIdx) + "_Port")
	);
}

void WvsLogin::CenterAliveMonitor(int nCenterIndex)
{
	if (m_apCenterInstance[nCenterIndex]->CheckSocketStatus(SocketBase::SocketStatus::eConnecting))
		return;
	WvsLogger::LogRaw(WvsLogger::LEVEL_WARNING, "--------------------Periodically reconnecting to WvsCenter--------------------\n");

	if (m_apCenterInstance[nCenterIndex] && (m_apCenterInstance[nCenterIndex]->CheckSocketStatus(SocketBase::SocketStatus::eClosed)))
	{
		WvsLogger::LogFormat("Failed connecting to Center Server %d, reconnection has been scheduled.\n", nCenterIndex);

		m_apCenterWorkThread[nCenterIndex]->detach();
		*m_apCenterWorkThread[nCenterIndex] = (std::thread(&WvsLogin::ConnectToCenter, this, nCenterIndex));
	}
}

void WvsLogin::InitializeCenter()
{
	m_nCenterCount = m_pCfgLoader->IntValue("CenterCount");
	for (int i = 0; i < m_nCenterCount; ++i)
	{
		aCenterServerService[i].reset(new asio::io_service());
		m_apCenterInstance[i] = std::make_shared<Center>(*aCenterServerService[i]);
		m_apCenterWorkThread[i].reset(new std::thread(&WvsLogin::ConnectToCenter, this, i));
		auto fHolderFunc = std::bind(&WvsLogin::CenterAliveMonitor, this, i);
		auto pAliveHolder = AsyncScheduler::CreateTask(fHolderFunc, 10 * 100, true);

		std::thread tCenterWorkThread([&] {
			asio::io_service::work work(*aCenterServerService[i]);
			std::error_code ec;
			aCenterServerService[i]->run(ec);
		});
		tCenterWorkThread.detach();
		pAliveHolder->Start();
	}
}

void WvsLogin::OnNotifySocketDisconnected(SocketBase *pSocket)
{
	auto pEntry = GetLoginEntryByLoginSocketSN(pSocket->GetSocketID());
	if(pEntry && pEntry->nLoginState != LoginState::LS_Stage_MigratedIn)
		RemoveLoginEntryByLoginSocketID(pSocket->GetSocketID());
}

void WvsLogin::RegisterLoginEntry(ZSharedPtr<LoginEntry> &pLoginEntry)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);

	//Restored entries have no uLoginSocketSN
	if(pLoginEntry->uLoginSocketSN)
		m_mSocketIDToAccountID[pLoginEntry->uLoginSocketSN] = pLoginEntry->nAccountID;
	m_mAccountIDToLoginEntry[pLoginEntry->nAccountID] = pLoginEntry;
}

void WvsLogin::RemoveLoginEntryByAccountID(int nAccountID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	auto pEntry = GetLoginEntryByAccountID(nAccountID);
	if (pEntry)
	{
		auto psEntry = GetLoginEntryByLoginSocketSN(pEntry->uLoginSocketSN);
		if (psEntry && (psEntry != pEntry || psEntry->nAccountID != nAccountID))
			WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "Inconsistent login entry, current AccountID = %d, existing data: [LoginSocketSN %u with AccountID: %d\n]", nAccountID, psEntry->uLoginSocketSN, psEntry->nAccountID);

		//Restored entries have no uLoginSocketSN
		if (pEntry->uLoginSocketSN)
			m_mSocketIDToAccountID.erase(pEntry->uLoginSocketSN);
		m_mAccountIDToLoginEntry.erase(nAccountID);
	}
}

void WvsLogin::RemoveLoginEntryByLoginSocketID(unsigned int uLoginSocketSN)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	auto pEntry = GetLoginEntryByLoginSocketSN(uLoginSocketSN);
	if (pEntry)
	{
		auto paEntry = GetLoginEntryByAccountID(pEntry->nAccountID);
		if(paEntry && (paEntry != pEntry || paEntry->uLoginSocketSN != uLoginSocketSN))
			WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "Inconsistent login entry, current LoginSocketSN = %d, existing data: [LoginSocketSN %u with AccountID: %d\n]", uLoginSocketSN, paEntry->uLoginSocketSN, paEntry->nAccountID);

		//Restored entries have no uLoginSocketSN
		if (pEntry->uLoginSocketSN)
			m_mSocketIDToAccountID.erase(uLoginSocketSN);
		m_mAccountIDToLoginEntry.erase(pEntry->nAccountID);
	}
}

ZSharedPtr<LoginEntry> WvsLogin::GetLoginEntryByAccountID(int nAccountID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	auto findIter = m_mAccountIDToLoginEntry.find(nAccountID);
	return findIter == m_mAccountIDToLoginEntry.end() ? nullptr : findIter->second;
}

ZSharedPtr<LoginEntry> WvsLogin::GetLoginEntryByLoginSocketSN(unsigned int uLoginSocketSN)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	auto findIter = m_mSocketIDToAccountID.find(uLoginSocketSN);
	if (findIter == m_mSocketIDToAccountID.end())
		return nullptr;
	auto accountIter = m_mAccountIDToLoginEntry.find(findIter->second);
	return accountIter == m_mAccountIDToLoginEntry.end() ? nullptr : accountIter->second;
}

void WvsLogin::RestoreLoginEntry(void * iPacket_)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	auto iPacket = (InPacket*)iPacket_;

	int nWorldID = iPacket->Decode1();
	int nCount = iPacket->Decode4();
	for (int i = 0; i < nCount; ++i)
	{
		ZSharedPtr<LoginEntry> pEntry(AllocObj(LoginEntry));
		pEntry->Initialize();
		pEntry->nWorldID = nWorldID;
		pEntry->nAccountID = iPacket->Decode4();
		pEntry->nLoginState = LoginState::LS_Stage_MigratedIn;

		RegisterLoginEntry(pEntry);
	}
}
