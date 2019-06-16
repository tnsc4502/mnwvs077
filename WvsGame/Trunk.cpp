#include "Trunk.h"

#ifdef _WVSGAME
#include "User.h"
#include "BasicStat.h"
#include "BackupItem.h"
#include "QWUInventory.h"
#include "WvsGame.h"
#endif 

#ifdef _WVSCENTER
#include "..\WvsCenter\WvsWorld.h"
#include "..\Database\TrunkDBAccessor.h"
#endif

#include "..\Database\GA_Character.hpp"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\GameSrvPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\FieldPacketFlags.hpp"
#include "..\WvsGame\ItemInfo.h"

Trunk::Trunk()
{
}

Trunk::~Trunk()
{
}

void Trunk::Encode(long long int liFlag, OutPacket *oPacket)
{
	oPacket->Encode1(m_nSlotCount);
	oPacket->Encode8(liFlag);
	if (liFlag & 2)
		oPacket->Encode4(m_nMoney);

	for (int nTI = 1; nTI <= 5; ++nTI)
		if (liFlag & ((nTI + 1) << 1))
		{
			oPacket->Encode1((char)m_aaItemSlot[nTI].size());
			for (auto p : m_aaItemSlot[nTI])
				p->RawEncode(oPacket);
		}
}

void Trunk::Decode(InPacket * iPacket)
{
	m_nSlotCount = iPacket->Decode1();
	iPacket->Decode8();
	m_nMoney = iPacket->Decode4();
	int nCount = 0;
	GW_ItemSlotBase* pItem = nullptr;
	for (int nTI = 1; nTI <= 5; ++nTI)
	{
		nCount = iPacket->Decode1();
		m_aaItemSlot[nTI].resize(nCount);
		for (int i = 0; i < nCount; ++i)
		{
			pItem = (nTI == GW_ItemSlotBase::EQUIP ?
				(GW_ItemSlotBase*)AllocObj(GW_ItemSlotEquip) : AllocObj(GW_ItemSlotBundle)
			);
			pItem->RawDecode(iPacket);
			m_aaItemSlot[nTI][i].reset(pItem);
		}
	}
}


#ifdef _WVSGAME
void Trunk::OnPacket(User *pUser, InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nRequest = iPacket->Decode1();
	switch (nRequest)
	{
		case TrunkRequest::rq_Trunk_MoveSlotToTrunk:
			OnMoveSlotToTrunkRequest(pUser, iPacket);
			break;
		case TrunkRequest::rq_Trunk_MoveTrunkToSlot:
			OnMoveTrunkToSlotRequest(pUser, iPacket);
			break;
		case TrunkRequest::rq_Trunk_Close:
			pUser->SetTrunk(nullptr);
			break;
		case TrunkRequest::rq_Trunk_WithdrawMoney:
			OnWithdrawMoney(pUser, iPacket);
			break;
	}
}

void Trunk::OnMoveSlotToTrunkRequest(User *pUser, InPacket *iPacket)
{
	int nPOS = iPacket->Decode2();
	int nItemID = iPacket->Decode4();
	int nNumber = iPacket->Decode2();
	int nTI = nItemID / 1000000;
	int nDecRet = 0;

	auto pItem = pUser->GetCharacterData()->GetItem(nTI, nPOS);
	if ((nTI == GW_ItemSlotBase::EQUIP && nNumber != 1) ||
		!pItem ||
		(nTI != GW_ItemSlotBase::EQUIP && ((GW_ItemSlotBundle*)pItem)->nNumber < nNumber))
	{
		pUser->SendNoticeMessage("發生異常，請稍後再試。");
		pUser->SendCharacterStat(true, 0);
		return;
	}

	std::vector<InventoryManipulator::ChangeLog> aLog;
	InventoryManipulator::RawRemoveItem(pUser->GetCharacterData(), nTI, nPOS, nNumber, &aLog, &nDecRet, &m_pTradingItem);
	QWUInventory::SendInventoryOperation(pUser, true, aLog);

	OutPacket oPacket;
	oPacket.Encode2(GameSrvSendPacketFlag::TrunkRequest);
	oPacket.Encode1(TrunkRequest::rq_Trunk_MoveSlotToTrunk);
	oPacket.Encode4(pUser->GetAccountID());
	oPacket.Encode4(pUser->GetUserID());
	oPacket.Encode1(nTI);
	oPacket.Encode2(nPOS);
	oPacket.Encode2(nNumber);
	pItem->Encode(&oPacket, true);
	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
}

void Trunk::OnMoveTrunkToSlotRequest(User *pUser, InPacket *iPacket)
{
	int nTI = iPacket->Decode1();
	int nPOS = iPacket->Decode1();
	if (nTI < GW_ItemSlotBase::EQUIP ||
		nTI > GW_ItemSlotBase::CASH ||
		nPOS >= (int)m_aaItemSlot[nTI].size())
	{
		pUser->SendNoticeMessage("發生異常，請稍後再試。");
		pUser->SendCharacterStat(true, 0);
		return;
	}
	auto pItem = m_aaItemSlot[nTI][nPOS];

	//Try taking item.
	std::vector<BackupItem> aBackup;
	int nInc = 0;
	bool bAdd = InventoryManipulator::RawAddItem(pUser->GetCharacterData(), nTI, pItem, nullptr, &nInc, &aBackup);
	InventoryManipulator::RestoreBackupItem(pUser->GetCharacterData(), aBackup);
	//InventoryManipulator::ReleaseBackupItem(aBackup);
	if (bAdd)
	{
		OutPacket oPacket;
		oPacket.Encode2(GameSrvSendPacketFlag::TrunkRequest);
		oPacket.Encode1(TrunkRequest::rq_Trunk_MoveTrunkToSlot);
		oPacket.Encode4(pUser->GetAccountID());
		oPacket.Encode4(pUser->GetUserID());
		oPacket.Encode1(nTI);
		oPacket.Encode1(nPOS);
		WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	}
	else
	{
		pUser->SendNoticeMessage("背包欄位空間不足。");
		pUser->SendCharacterStat(true, 0);
		return;
	}
}

void Trunk::OnLoadDone(User *pUser, InPacket *iPacket)
{
	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketFlag::Field_TrunkRequest);
	oPacket.Encode1(Trunk::TrunkResult::res_Trunk_Load);
	oPacket.Encode4(m_nTrunkTemplateID);
	Encode(0xFFFF, &oPacket);
	pUser->SendPacket(&oPacket);
}

void Trunk::OnMoveSlotToTrunkDone(User *pUser, InPacket *iPacket)
{
	int nTI = iPacket->Decode1();
	int nPOS = iPacket->Decode2();
	int nNumber = iPacket->Decode2();

	m_aaItemSlot[nTI].push_back(m_pTradingItem);
	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketFlag::Field_TrunkRequest);
	oPacket.Encode1(Trunk::TrunkResult::res_Trunk_MoveSlotToTrunk);
	Encode(0xFFFF, &oPacket);
	pUser->SendPacket(&oPacket);
}

void Trunk::OnMoveTrunkToSlotDone(User *pUser, InPacket *iPacket)
{
	int nTI = iPacket->Decode1();
	int nPOS = iPacket->Decode1();
	std::vector<InventoryManipulator::ChangeLog> aLog;
	std::vector<BackupItem> aBackup;

	auto pItem = m_aaItemSlot[nTI][nPOS];
	int nInc = 0;
	bool bAdd = InventoryManipulator::RawAddItem(pUser->GetCharacterData(), nTI, pItem, &aLog, &nInc, &aBackup);
	if (!bAdd)
	{
		InventoryManipulator::RestoreBackupItem(pUser->GetCharacterData(), aBackup);
		//InventoryManipulator::ReleaseBackupItem(aBackup);

		//Force put back item.
		OutPacket oPacket;
		oPacket.Encode2(GameSrvSendPacketFlag::TrunkRequest);
		oPacket.Encode1(TrunkRequest::rq_Trunk_MoveSlotToTrunk);
		oPacket.Encode4(pUser->GetAccountID());
		oPacket.Encode4(pUser->GetUserID());
		oPacket.Encode1(nTI);
		oPacket.Encode2(GW_ItemSlotBase::LOCK_POS);
		oPacket.Encode2(nTI == GW_ItemSlotBase::EQUIP ? 1 : ((GW_ItemSlotBundle*)pItem)->nNumber);
		pItem->Encode(&oPacket, true);
		WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	}
	else
	{
		m_aaItemSlot[nTI].erase(m_aaItemSlot[nTI].begin() + nPOS);
		QWUInventory::SendInventoryOperation(pUser, true, aLog);

		OutPacket oPacket;
		oPacket.Encode2(FieldSendPacketFlag::Field_TrunkRequest);
		oPacket.Encode1(Trunk::TrunkResult::res_Trunk_MoveTrunkToSlot);
		Encode(0xFFFF, &oPacket);
		pUser->SendPacket(&oPacket);
		pUser->FlushCharacterData();
	}
}

void Trunk::OnWithdrawMoneyDone(User *pUser, InPacket *iPacket)
{
	int nResult = iPacket->Decode1();
	if (nResult != 1)
		pUser->SendNoticeMessage("發生異常。");
	else 
	{
		m_nSlotCount = iPacket->Decode1();
		m_nMoney = iPacket->Decode4();

		OutPacket oPacket;
		oPacket.Encode2(FieldSendPacketFlag::Field_TrunkRequest);
		oPacket.Encode1(Trunk::TrunkResult::res_Trunk_MoveSlotToTrunk);
		Encode(2, &oPacket);
		pUser->SendPacket(&oPacket);
	}
	pUser->SendCharacterStat(true, BasicStat::BS_Meso);
}

void Trunk::OnWithdrawMoney(User *pUser, InPacket *iPacket)
{
	int nMoney = iPacket->Decode4();
	if (nMoney <= m_nMoney)
	{
		if (QWUInventory::Exchange(pUser, nMoney, std::vector<ExchangeElement>{}))
			pUser->SendNoticeMessage("交易失敗，可能是楓幣不足或已滿。");
		else
			m_nMoney += (-nMoney);

		OutPacket oPacket;
		oPacket.Encode2(GameSrvSendPacketFlag::TrunkRequest);
		oPacket.Encode1(TrunkRequest::rq_Trunk_WithdrawMoney);
		oPacket.Encode4(pUser->GetAccountID());
		oPacket.Encode4(pUser->GetUserID());
		oPacket.Encode1(m_nSlotCount);
		oPacket.Encode4(m_nMoney);
		WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	}
}
#endif

//CENTER
#ifdef _WVSCENTER

Trunk* Trunk::Load(int nAccountID)
{
	Trunk *pRet = AllocObj(Trunk);
	auto prTrunk = TrunkDBAccessor::LoadTrunk(nAccountID);
	pRet->m_nSlotCount = prTrunk.first;
	pRet->m_nMoney = prTrunk.second;
	pRet->m_aaItemSlot[GW_ItemSlotBase::EQUIP] = TrunkDBAccessor::LoadTrunkEquip(nAccountID);
	pRet->m_aaItemSlot[GW_ItemSlotBase::CONSUME] = TrunkDBAccessor::LoadTrunkConsume(nAccountID);
	pRet->m_aaItemSlot[GW_ItemSlotBase::ETC] = TrunkDBAccessor::LoadTrunkEtc(nAccountID);
	pRet->m_aaItemSlot[GW_ItemSlotBase::INSTALL] = TrunkDBAccessor::LoadTrunkInstall(nAccountID);

	return pRet;
}

void Trunk::MoveSlotToTrunk(int nAccountID, InPacket *iPacket)
{
	int nCharacterID = iPacket->Decode4();
	int nTI = iPacket->Decode1();
	ZSharedPtr<GW_ItemSlotBase> pItem = (nTI == GW_ItemSlotBase::EQUIP ?
		(GW_ItemSlotBase*)AllocObj(GW_ItemSlotEquip) : AllocObj(GW_ItemSlotBundle)
		);
	iPacket->Decode2(); //nPOS
	int nNumber = iPacket->Decode2();
	int nPOS = iPacket->Decode1();
	pItem->nType = (GW_ItemSlotBase::GW_ItemSlotType)nTI;
	pItem->nPOS = nPOS;
	pItem->Decode(iPacket, true);
	//int nPOS = pItem->nPOS;

	if (nTI != GW_ItemSlotBase::EQUIP && 
		!ItemInfo::GetInstance()->IsRechargable(pItem->nItemID) &&
		((GW_ItemSlotBundle*)pItem)->nNumber > nNumber)
	{
		((GW_ItemSlotBundle*)pItem)->nNumber -= nNumber;
		pItem->Save(nCharacterID);
		pItem.reset(pItem->MakeClone());
		((GW_ItemSlotBundle*)pItem)->nNumber = nNumber;
	}
	pItem->nCharacterID = -1;
	pItem->nPOS = GW_ItemSlotBase::LOCK_POS;
	pItem->Save(-1);
	TrunkDBAccessor::MoveSlotToTrunk(nAccountID, pItem->liItemSN, nTI);

	OutPacket oPacket;
	oPacket.Encode2(CenterSendPacketFlag::TrunkResult);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode1(TrunkResult::res_Trunk_MoveSlotToTrunk);
	oPacket.Encode1(nTI);
	oPacket.Encode2(nPOS);
	oPacket.Encode2(nNumber);
	pItem->RawEncode(&oPacket);

	auto pwUser = WvsWorld::GetInstance()->GetUser(nCharacterID);
	if (pwUser)
		pwUser->SendPacket(&oPacket);
}

void Trunk::MoveTrunkToSlot(int nAccountID, InPacket *iPacket)
{
	int nCharacterID = iPacket->Decode4();
	int nTI = iPacket->Decode1();
	int nPOS = iPacket->Decode1();
	auto pItem = m_aaItemSlot[nTI][nPOS];
	auto liItemSN = pItem->liItemSN;

	bool bTreatSingly = nTI == GW_ItemSlotBase::EQUIP || ItemInfo::GetInstance()->IsRechargable(pItem->nItemID);
	pItem->nPOS = GW_ItemSlotBase::LOCK_POS;

	//If the target item is an equip or rechargeable, then just update it's owner ID.
	//Otherwise, just remove it from DB and let it be automatically inserted next time saving the character.
	if (bTreatSingly)
		pItem->Save(nCharacterID);
	else
		pItem->liItemSN = -1;

	TrunkDBAccessor::MoveTrunkToSlot(nAccountID, liItemSN, nTI, bTreatSingly);
	OutPacket oPacket;
	oPacket.Encode2(CenterSendPacketFlag::TrunkResult);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode1(TrunkResult::res_Trunk_MoveTrunkToSlot);
	oPacket.Encode1(nTI);
	oPacket.Encode1(nPOS);

	auto pwUser = WvsWorld::GetInstance()->GetUser(nCharacterID);
	if (pwUser)
		pwUser->SendPacket(&oPacket);
}

void Trunk::WithdrawMoney(int nAccountID, InPacket * iPacket)
{
	int nCharacterID = iPacket->Decode4();
	int nSlotCount = iPacket->Decode1();
	int nMoney = iPacket->Decode4();

	TrunkDBAccessor::UpdateTrunk(nAccountID, nMoney, nSlotCount);
	auto prTrunk = TrunkDBAccessor::LoadTrunk(nAccountID);

	OutPacket oPacket;
	oPacket.Encode2(CenterSendPacketFlag::TrunkResult);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode1(TrunkResult::res_Trunk_WithdrawMoney);
	oPacket.Encode1(1);
	oPacket.Encode1(prTrunk.first);
	oPacket.Encode4(prTrunk.second);

	auto pwUser = WvsWorld::GetInstance()->GetUser(nCharacterID);
	if (pwUser)
		pwUser->SendPacket(&oPacket);
}

#endif