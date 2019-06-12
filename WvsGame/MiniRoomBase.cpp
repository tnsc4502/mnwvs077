#include "MiniRoomBase.h"
#include "QWUser.h"
#include "User.h"
#include "Field.h"
#include "TradingRoom.h"
#include "PersonalShop.h"
#include "EntrustedShop.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\FieldPacketFlags.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\Database\GW_ItemSlotBase.h"

std::atomic<int> MiniRoomBase::ms_nSNCounter = 1;
std::map<int, MiniRoomBase*> MiniRoomBase::ms_mMiniRoom;
std::mutex MiniRoomBase::ms_mtxMiniRoomLock;

MiniRoomBase::MiniRoomBase(int nMaxUsers)
{
	m_nMiniRoomSN = ++ms_nSNCounter;
	m_sTitle = "";
	m_sPassword = "";
	m_nMaxUsers = nMaxUsers;
	m_nCurUsers = 0;
	m_bOpened = false;
	m_bCloseRequest = false;
	m_bTournament = false;
	m_bGameOn = false;

	for (int i = 0; i < MAX_USER_NUM; ++i) 
	{
		m_anReservedTime[i] = 0;
		m_anReserved[i] = 0;
		m_anLeaveRequest[i] = -1;
		m_apUser[i] = nullptr;
	}
}

MiniRoomBase::~MiniRoomBase()
{
}

int MiniRoomBase::GetMiniRoomSN() const
{
	return m_nMiniRoomSN;
}

int MiniRoomBase::GetMaxUsers() const
{
	return m_nMaxUsers;
}

int MiniRoomBase::GetCurUsers() const
{
	return m_nCurUsers;
}

int MiniRoomBase::GetRound() const
{
	return m_nRound;
}

int MiniRoomBase::GetMiniRoomSpec() const
{
	return m_nMiniRoomSpec;
}

void MiniRoomBase::Broadcast(OutPacket * oPacket, User *pExcept)
{
	oPacket->GetSharedPacket()->ToggleBroadcasting();

	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	for (int i = 0; i < m_nMaxUsers; ++i)
	{
		if (m_apUser[i] && m_apUser[i] != pExcept)
			m_apUser[i]->SendPacket(oPacket);
	}
}

void MiniRoomBase::SetTitle(const std::string &sTitle)
{
	m_sTitle = sTitle;
}

const std::string& MiniRoomBase::GetTitle() const
{
	return m_sTitle;
}

void MiniRoomBase::SetPassword(const std::string &sPassword)
{
	m_sPassword = sPassword;
}

bool MiniRoomBase::CheckPassword(const std::string &sPassword)
{
	return sPassword == m_sPassword;
}

bool MiniRoomBase::IsGameOn() const
{
	return m_bGameOn;
}

bool MiniRoomBase::IsPrivate() const
{
	return m_bPrivate;
}

bool MiniRoomBase::IsOpened() const
{
	return m_bOpened;
}

bool MiniRoomBase::IsEmployer(User *pUser) const
{
	return false;
}

bool MiniRoomBase::IsEntrusted() const
{
	return (m_nType == MiniRoomType::e_MiniRoom_EntrustedShop);
}

int MiniRoomBase::GetEmployeeTemplateID() const
{
	return 0;
}

const std::string& MiniRoomBase::GetEmployeeName() const
{
	return "";
}

void MiniRoomBase::RemoveMiniRoom()
{
	std::lock_guard<std::mutex> lock(ms_mtxMiniRoomLock);
	ms_mMiniRoom.erase(m_nMiniRoomSN);

	WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "Remove MiniRoom!\n");
	this->Release();
}

int MiniRoomBase::GetLeaveType() const
{
	return 0;
}

int MiniRoomBase::GetType() const
{
	return m_nType;
}

unsigned char MiniRoomBase::FindEmptySlot(int nCharacterID)
{
	for (int i = 0; i < m_nMaxUsers; ++i)
		if (i == 0 && IsEntrusted()) //Position 0 is always reserved for OWNER.
			continue;
		else if (GameDateTime::GetTime() - 30 * 1000 > m_anReservedTime[i])
			m_anReserved[i] = m_anReservedTime[i] = 0;

	for (int i = 0; i < m_nMaxUsers; ++i)
		if (!m_apUser[i] && 
			(m_anReserved[i] == 0 || m_anReserved[i] == nCharacterID))
			return i;
	return -1;
}

User* MiniRoomBase::FindUser(User *pUser)
{
	for (int i = 0; i < m_nMaxUsers; ++i)
		if (m_apUser[i] == pUser)
			return pUser;
	return nullptr;
}

int MiniRoomBase::FindUserSlot(User * pUser)
{
	for (int i = 0; i < m_nMaxUsers; ++i)
		if (m_apUser[i] == pUser)
			return i;
	return -1;
}

void MiniRoomBase::OnPacketBase(User *pUser, int nType, InPacket *iPacket)
{
	switch (nType)
	{
		case MiniRoomRequest::rq_MiniRoom_InviteBase:
			OnInviteBase(pUser, iPacket);
			break;
		case MiniRoomRequest::rq_MiniRoom_Chat:
			OnChat(pUser, iPacket, -1);
			break;
		case MiniRoomRequest::rq_MiniRoom_LeaveBase:
			OnLeaveBase(pUser, iPacket);
			break;
		case MiniRoomRequest::rq_MiniRoom_BalloonBase:
			OnBalloonBase(pUser, iPacket);
			break;
		default:
			pUser->GetMiniRoom()->OnPacket(pUser, nType, iPacket);
			break;
	}
	ProcessLeaveRequest();
}

void MiniRoomBase::OnChat(User *pUser, InPacket *iPacket, int nMessageCode)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	int nIdx = FindUserSlot(pUser);
	if (m_nCurUsers && nIdx >= 0)
	{
		OutPacket oPacket;
		oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
		oPacket.Encode1(MiniRoomRequest::rq_MiniRoom_Chat);
		if (nMessageCode < 0)
		{
			oPacket.Encode1(8); //nMessageType
			oPacket.Encode1(nIdx);
			oPacket.EncodeStr(pUser->GetName() + " : " + iPacket->DecodeStr());
		}
		else
		{

		}
		Broadcast(&oPacket, nullptr);
	}
}

unsigned char MiniRoomBase::OnCreateBase(User *pUser, InPacket *iPacket, int nRound)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	if (pUser->CanAttachAdditionalProcess())
	{
		pUser->SetMiniRoom(this);
		int nAdmissionRet = IsAdmitted(pUser, iPacket, true);
		if (nAdmissionRet)
		{
			pUser->SetMiniRoom(nullptr);
			return nAdmissionRet;
		}
		m_nCurUsers = 1;
		m_nRound = nRound;
		m_anLeaveRequest[0] = -1;
		m_apUser[0] = pUser;

		std::lock_guard<std::mutex> lock(ms_mtxMiniRoomLock);
		ms_mMiniRoom[m_nMiniRoomSN] = this;
		return MiniRoomCreateResult::res_Create_Success;
	}
	return MiniRoomCreateResult::res_Create_Failed_UnableToProcess;
}

void MiniRoomBase::OnInviteBase(User *pUser, InPacket *iPacket)
{
	auto pToInvite = User::FindUser(iPacket->Decode4());
	int nFailedReason = 0;

	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	if (!pToInvite || pToInvite == pUser)
		nFailedReason = MiniRoomInviteResult::res_Invite_InvalidUser;
	else if (!pToInvite || !pToInvite ->CanAttachAdditionalProcess() || m_nCurUsers == m_nMaxUsers)
		nFailedReason = MiniRoomInviteResult::res_Invite_UnableToProcess;

	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
	if (nFailedReason)
	{
		oPacket.Encode1(MiniRoomRequest::rq_MiniRoom_MRInviteResult);
		oPacket.Encode1(nFailedReason);
		if (nFailedReason == MiniRoomInviteResult::res_Invite_UnableToProcess)
			oPacket.EncodeStr(pToInvite->GetName());
		pUser->SendPacket(&oPacket);
	}
	else
	{
		oPacket.Encode1(MiniRoomRequest::rq_MiniRoom_InviteBase);
		oPacket.Encode1((char)m_nType);
		oPacket.EncodeStr(pUser->GetName());
		oPacket.Encode4(m_nMiniRoomSN);
		pToInvite->SendPacket(&oPacket);
	}
}

void MiniRoomBase::OnInviteResult(User * pUser, int nResult)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	if (m_apUser[0])
	{
		OutPacket oPacket;
		oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
		oPacket.Encode1(MiniRoomRequest::rq_MiniRoom_MRInviteResult);
		oPacket.Encode1(nResult);
		oPacket.EncodeStr(pUser->GetName());
		m_apUser[0]->SendPacket(&oPacket);

		CloseRequest(m_apUser[0], nResult, nResult);
		ProcessLeaveRequest();
	}
}

void MiniRoomBase::OnLeaveBase(User *pUser, InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	int nIdx = FindUserSlot(pUser);
	if (m_nCurUsers && nIdx >= 0)
	{
		if (m_nType == MiniRoomType::e_MiniRoom_MemoryGame)
			CloseRequest(pUser, 2, 0);
		else if (m_nType != MiniRoomType::e_MiniRoom_Omok || nIdx)
		{
			DoLeave(nIdx, 0, true);
			if (m_bOpened
				&& (m_nType != 4
					&& m_nType != 5
					|| m_nType == 4 && m_nCurUsers == m_nMaxUsers - 1
					|| m_nType == 5 && m_nCurUsers == m_nMaxUsers - 2))
			{
			}
		}
		else
			CloseRequest(pUser, 3, 3);
	}
}

void MiniRoomBase::OnBalloonBase(User *pUser, InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	int nIdx = FindUserSlot(pUser);
	if (nIdx == -1)
	{
		pUser->SendCharacterStat(true, 0);
		return;
	}

	m_bOpened = true;
	if (IsEntrusted())
	{
		pUser->CreateEmployee(iPacket->Decode1() == 1);
		CloseRequest(pUser, -1, 12);
	}
	else
	{
		pUser->SetMiniRoomBalloon(iPacket->Decode1() == 1);
	}
}

void MiniRoomBase::OnLeave(User * pUser, int nLeaveType)
{
}

void MiniRoomBase::DoLeave(int nIdx, int nLeaveType, bool bBroadcast)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	
	auto pUser = m_apUser[nIdx];
	if (pUser && pUser->GetField())
	{
		std::lock_guard<std::recursive_mutex> lockUser(pUser->GetLock());
		OnLeave(pUser, nLeaveType);
		if (nLeaveType)
		{
			OutPacket oPacket;
			oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
			oPacket.Encode1(MiniRoomRequest::rq_MiniRoom_LeaveBase);
			oPacket.Encode1(nIdx);
			oPacket.Encode1(nLeaveType);
			pUser->SendPacket(&oPacket);
		}
		pUser->SetMiniRoom(nullptr);
		m_apUser[nIdx] = 0;
		if(!IsEmployer(pUser))
			--m_nCurUsers;

		if (bBroadcast)
		{
			OutPacket oPacket;
			oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
			oPacket.Encode1(MiniRoomRequest::rq_MiniRoom_LeaveBase);
			oPacket.Encode1(nIdx);
			oPacket.Encode1(GetLeaveType());
			EncodeLeave(pUser, &oPacket);
			Broadcast(&oPacket, pUser);
		}

		if ((IsEntrusted() && nLeaveType == 3) ||
			(!IsEntrusted() && !m_nCurUsers))
			RemoveMiniRoom();
	}
}

unsigned char MiniRoomBase::OnEnterBase(User *pUser, InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);

	unsigned char nIdx = FindEmptySlot(pUser->GetUserID());
	auto pFind = FindUser(pUser);

	if (pFind)
		return MiniRoomEnterResult::res_Enter_AlreadyIn;
	else if (nIdx == -1)
		return MiniRoomEnterResult::res_Enter_NoEmptySlot;
	
	if (!pUser->CanAttachAdditionalProcess())
		return MiniRoomEnterResult::res_Enter_UnableToProcess;

	pUser->SetMiniRoom(this);
	int nFailedReason = IsAdmitted(pUser, iPacket, false);
	if (nFailedReason)
	{
		pUser->SetMiniRoom(nullptr);
		return nFailedReason;
	}
	else
	{
		m_apUser[nIdx] = pUser;
		if (IsEmployer(pUser))
		{
			m_bOpened = false;
			for (int i = 1; i < m_nCurUsers; ++i)
				if (m_apUser[i])
					m_anLeaveRequest[i] = 14;
			ProcessLeaveRequest();
		}
		else
		{
			++m_nCurUsers;
			m_anReserved[nIdx] = 0;
			m_anLeaveRequest[nIdx] = -1;
			OutPacket oPacket;
			oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
			oPacket.Encode1(MiniRoomRequest::rq_MiniRoom_MREnter);
			EncodeAvatar(nIdx, &oPacket);
			EncodeEnter(pUser, &oPacket);
			Broadcast(&oPacket, pUser);
		}
		OutPacket oPacket;
		oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
		oPacket.Encode1(MiniRoomRequest::rq_MiniRoom_MRCreateResult);
		oPacket.Encode1(m_nType);
		oPacket.Encode1(m_nMaxUsers);
		oPacket.Encode1(nIdx);
		for (int i = 0; i < m_nCurUsers; ++i)
			EncodeAvatar(i, &oPacket);
		oPacket.Encode1((char)0xFF);
		EncodeEnterResult(pUser, &oPacket);
		pUser->SendPacket(&oPacket);

		if (m_bTournament)
			SetTitle(m_apUser[1]->GetName() + " VS " + m_apUser[0]->GetName());
		
		return 0;
	}
}

int MiniRoomBase::IsAdmitted(User *pUser, InPacket *iPacket, bool bOnCreate)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	if (!QWUser::GetHP(pUser))
		return MiniRoomAdmissionResult::res_Admission_InvalidUserStat; //InvalidUserStat

	int nFieldID = 0;
	if (pUser->GetField())
		nFieldID = pUser->GetField()->GetFieldID();

	if (m_bTournament)
	{
		if (bOnCreate)
			m_bPrivate = false;
		return MiniRoomAdmissionResult::res_Admission_Success;
	}

	if (((nFieldID / 1000000) % 100) == 9)
		return MiniRoomAdmissionResult::res_Admission_InvalidFieldID; //InvalidFieldID

	/*if((m_nType == MiniRoomType::e_MiniRoom_PersonalShop || m_nType == MiniRoomType::e_MiniRoom_EntrustedShop) &&
		(!nFieldID)*/ //Check if the field is able to create personal shops.

	/*if((m_nType == MiniRoomType::e_MiniRoom_Omok || m_nType == MiniRoomType::e_MiniRoom_MemoryGame) &&
		(!nFieldID) || )*/ //Check if the field is able to create games.

	if (!bOnCreate)
	{
		if ((m_nType == MiniRoomType::e_MiniRoom_PersonalShop || m_nType == MiniRoomType::e_MiniRoom_EntrustedShop) 
			&& !m_bOpened)
			return MiniRoomAdmissionResult::res_Admission_InvalidShopStatus;

		if (iPacket->Decode1())
		{
			if (!m_bPrivate || CheckPassword(iPacket->DecodeStr()))
				return MiniRoomAdmissionResult::res_Admission_Success;
		}
		else if (!m_bPrivate)
			return MiniRoomAdmissionResult::res_Admission_Success;
		else
			return MiniRoomAdmissionResult::res_Admission_InvalidPassword; //InvalidPassword
	}
	else
		m_bPrivate = (iPacket->Decode1() == 1);

	if (m_bPrivate)
		SetPassword(iPacket->DecodeStr());

	return MiniRoomAdmissionResult::res_Admission_Success;
}

void MiniRoomBase::CloseRequest(User *pUser, int nLeaveType, int nLeaveType2)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	for (int i = 0; i < m_nMaxUsers; ++i)
	{
		if (m_apUser[i] == pUser)
			m_anLeaveRequest[i] = nLeaveType;
		else
			m_anLeaveRequest[i] = nLeaveType2;
	}
}

void MiniRoomBase::ProcessLeaveRequest()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	for (int i = 0; i < m_nMaxUsers; ++i)
	{
		if (m_apUser[i] && m_anLeaveRequest[i] >= 0)
		{
			DoLeave(i, m_anLeaveRequest[i], 0);
		}
	}
}

void MiniRoomBase::EncodeAvatar(int nIdx, OutPacket *oPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	if (nIdx || !IsEntrusted())
	{
		if (m_apUser[nIdx])
		{
			oPacket->Encode1(nIdx);
			m_apUser[nIdx]->EncodeAvatar(oPacket);
			oPacket->EncodeStr(m_apUser[nIdx]->GetName());
		}
	}
	else
	{
		oPacket->Encode1(nIdx);
		oPacket->Encode4(GetEmployeeTemplateID());
		oPacket->EncodeStr("ºëÆF°Ó¤H"); //g_sd(StringDecoder)->GetString(3132)
	}
}

void MiniRoomBase::EncodeEnter(User *pUser, OutPacket *oPacket)
{
}

void MiniRoomBase::EncodeEnterResult(User *pUser, OutPacket *oPacket)
{
}

void MiniRoomBase::EncodeLeave(User * pUser, OutPacket * oPacket)
{
}

void MiniRoomBase::Enter(User *pUser, int nSN, InPacket *iPacket, bool bTournament)
{
	auto pMiniRoom = GetMiniRoom(nSN);
	int nFailedReason = 0;
	if (pMiniRoom)
	{
		if (pMiniRoom->m_bTournament == bTournament)
			nFailedReason = pMiniRoom->OnEnterBase(pUser, iPacket);
		else
			nFailedReason = 16;
	}
	else
		nFailedReason = 1;

	if (nFailedReason)
	{
		OutPacket oPacket;
		oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
		oPacket.Encode1(MiniRoomRequest::rq_MiniRoom_MRCreateResult); //EnterFailed
		oPacket.Encode1(0);
		oPacket.Encode1(nFailedReason);
		pUser->SendPacket(&oPacket);
	}
}

void MiniRoomBase::InviteResult(User *pUser, int nSN, int nResult)
{
	auto pMiniRoom = GetMiniRoom(nSN);
	if (pMiniRoom)
		pMiniRoom->OnInviteResult(pUser, nResult);
}

MiniRoomBase* MiniRoomBase::Create(User *pUser, int nMiniRoomType, InPacket *iPacket, bool bTournament, int nRound)
{
	auto pMiniRoom = MiniRoomFactory(nMiniRoomType, iPacket, bTournament);
	if (!pMiniRoom) 
		return nullptr; //Error.

	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
	oPacket.Encode1(MiniRoomBase::MiniRoomRequest::rq_MiniRoom_MRCreateResult);

	try 
	{
		char nResult = pMiniRoom->OnCreateBase(pUser, iPacket, nRound);
		if (nResult)
		{
			oPacket.Encode1(0);
			oPacket.Encode1(nResult);
			pUser->SendPacket(&oPacket);
			pMiniRoom->Release();
			return nullptr;
		}
		else
		{
			oPacket.Encode1((char)pMiniRoom->GetType());
			oPacket.Encode1((char)pMiniRoom->GetMaxUsers());
			oPacket.Encode1(0); //nIdx
			pMiniRoom->EncodeAvatar(0, &oPacket);
			oPacket.Encode1((char)0xFF);
			pMiniRoom->EncodeEnterResult(pUser, &oPacket);
			pUser->SendPacket(&oPacket);
		}
	}
	catch (std::exception& e)
	{
		FreeObj(pMiniRoom);
		throw e;
	}

	return pMiniRoom;
}

MiniRoomBase* MiniRoomBase::MiniRoomFactory(int nMiniRoomType, InPacket *iPacket, bool bTournament)
{
	MiniRoomBase* pRet = nullptr;
	if (nMiniRoomType == MiniRoomType::e_MiniRoom_Omok)
	{
	}
	else if (nMiniRoomType == MiniRoomType::e_MiniRoom_MemoryGame)
	{
	}
	else if (nMiniRoomType == MiniRoomType::e_MiniRoom_TradingRoom)
	{
		pRet = AllocObj(TradingRoom);
		pRet->m_nType = MiniRoomType::e_MiniRoom_TradingRoom;
	}
	else if (nMiniRoomType == MiniRoomType::e_MiniRoom_PersonalShop)
	{
		pRet = AllocObj(PersonalShop);
		pRet->m_nType = MiniRoomType::e_MiniRoom_PersonalShop;
		pRet->m_sTitle = iPacket->DecodeStr();
	}
	else if (nMiniRoomType == MiniRoomType::e_MiniRoom_EntrustedShop)
	{
		pRet = AllocObj(EntrustedShop);
		pRet->m_nType = MiniRoomType::e_MiniRoom_EntrustedShop;
		pRet->m_sTitle = iPacket->DecodeStr();	
	}

	return pRet;
}

MiniRoomBase * MiniRoomBase::GetMiniRoom(int nSN)
{
	std::lock_guard<std::mutex> lock(ms_mtxMiniRoomLock);

	auto findIter = ms_mMiniRoom.find(nSN);
	return findIter == ms_mMiniRoom.end() ? nullptr : findIter->second;
}
