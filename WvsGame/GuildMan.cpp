#include "GuildMan.h"
#include "WvsGame.h"
#include "User.h"
#include "QWUser.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\GameSrvPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

#ifdef _WVSCENTER
#include "..\WvsCenter\WvsCenter.h"
#include "..\WvsCenter\WvsWorld.h"
#include "..\Database\GuildDBAccessor.h"
#endif

GuildMan::GuildMan()
{
#ifdef _WVSCENTER
	m_atiGuildIDCounter = GuildDBAccessor::GetInstance()->GetGuildIDCounter();
#endif
}

GuildMan::~GuildMan()
{
}

int GuildMan::GetGuildIDByCharID(int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);

	auto findIter = m_mCharIDToGuildID.find(nCharacterID);
	return findIter == m_mCharIDToGuildID.end() ? -1 : findIter->second;
}

GuildMan::GuildData *GuildMan::GetGuildByCharID(int nCharacterID)
{
	int nGuildID = GetGuildIDByCharID(nCharacterID);
	
	return nGuildID == -1 ? nullptr : GetGuild(nGuildID);
}

GuildMan::GuildData *GuildMan::GetGuild(int nGuildID)
{
	if (nGuildID == -1)
		return nullptr;

	std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
	auto findIter = m_mGuild.find(nGuildID);
	return findIter == m_mGuild.end() ? nullptr : findIter->second;
}

GuildMan::GuildData * GuildMan::GetGuildByName(const std::string & strName)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);

	auto findIter = m_mNameToGuild.find(strName);
	return findIter == m_mNameToGuild.end() ? nullptr : findIter->second;
}

int GuildMan::FindUser(int nCharacterID, GuildData * pData)
{
	if (!pData)
		return -1;
	std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
	int nCount = (int)pData->anCharacterID.size();

	for (int i = 0; i < nCount; ++i)
		if (pData->anCharacterID[i] == nCharacterID)
			return i;
	return -1;
}

GuildMan * GuildMan::GetInstance()
{
	static GuildMan* pInstance = new GuildMan();

	return pInstance;
}

#ifdef _WVSGAME

void GuildMan::OnLeave(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);

	m_mCharIDToGuildID.erase(pUser->GetUserID());
}

void GuildMan::Broadcast(OutPacket *oPacket, const std::vector<int>& anMemberID, int nPlusOne)
{
	oPacket->GetSharedPacket()->ToggleBroadcasting();
	if (nPlusOne >= 0)
	{
		auto pUser = User::FindUser(nPlusOne);
		if (pUser)
			pUser->SendPacket(oPacket);
	}

	for (auto& nID : anMemberID)
	{
		if (nID * -1 == nPlusOne)
			continue;
		auto pUser = User::FindUser(nID);
		if (pUser)
			pUser->SendPacket(oPacket);
	}
}

void GuildMan::OnPacket(InPacket * iPacket)
{
	int nResult = iPacket->Decode1();
	switch (nResult)
	{
		case GuildMan::GuildResult::res_Guild_Load:
			OnGuildLoadDone(iPacket);
			break;
		case GuildMan::GuildResult::res_Guild_Create:
			OnCreateNewGuildDone(iPacket);
			break;
		case GuildMan::GuildResult::res_Guild_Join:
			OnJoinGuildDone(iPacket);
			break;
		case GuildMan::GuildResult::res_Guild_Notify_LoginOrLogout:
			OnNotifyLoginOrLogout(iPacket);
			break;
		case GuildMan::GuildResult::res_Guild_Withdraw:
			OnWithdrawGuildDone(iPacket);
			break;
		case GuildMan::GuildResult::res_Guild_SetGradeName:
			OnSetGradeNameDone(iPacket);
			break;
		case GuildMan::GuildResult::res_Guild_SetNotice:
			OnSetNoticeDone(iPacket);
			break;
		case GuildMan::GuildResult::res_Guild_SetMark:
			OnSetMarkDone(iPacket);
			break;
		case GuildMan::GuildResult::res_Guild_SetMemberGrade:
			OnSetMemberGradeDone(iPacket);
			break;
		case GuildMan::GuildResult::res_Guild_IncMaxMemberNum:
			break;
		case GuildMan::GuildResult::res_Guild_IncPoint:
			break;
	}
}

void GuildMan::OnGuildReuqest(User * pUser, InPacket * iPacket)
{
	int nRequest = iPacket->Decode1();
	switch (nRequest)
	{
		case GuildMan::GuildRequest::rq_Guild_Invite:
			OnGuildInviteRequest(pUser, iPacket);
			break;
		case GuildMan::GuildRequest::rq_Guild_Join:
			OnJoinGuildReqest(pUser, iPacket);
			break;
		case GuildMan::GuildRequest::rq_Guild_Withdraw:
		case GuildMan::GuildRequest::rq_Guild_Withdraw_Kick:
			OnWithdrawGuildRequest(pUser, iPacket);
			break;
		case GuildMan::GuildRequest::rq_Guild_SetGradeName:
			OnSetGradeNameRequest(pUser, iPacket);
			break;
		case GuildMan::GuildRequest::rq_Guild_SetNotice:
			OnSetNoticeRequest(pUser, iPacket);
			break;
		case GuildMan::GuildRequest::rq_Guild_SetMemberGrade:
			OnSetMemberGradeRequest(pUser, iPacket);
			break;
		case GuildMan::GuildRequest::rq_Guild_SetMark:
			OnSetMarkRequest(pUser, iPacket);
			break;
			
	}
}

void GuildMan::OnGuildLoadDone(InPacket * iPacket)
{
	int nUserID = iPacket->Decode4();
	int nGuildID = iPacket->Decode4();
	auto pGuild = GetGuildByCharID(nUserID);

	std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
	//Not in the guild
	if (pGuild || nGuildID == -1)
	{
		//Kicked
		if (nGuildID == -1 && pGuild)
			RemoveUser(pGuild, nUserID);
		else if(nGuildID == -1 && ((pGuild = GetGuild(nGuildID)), pGuild))
		{
			for (auto& nID : pGuild->anCharacterID)
				m_mCharIDToGuildID.erase(nID);
			m_mGuild.erase(pGuild->nGuildID);
			FreeObj(pGuild);
		}
		return;
	}

	pGuild = GetGuild(nGuildID);
	if (!pGuild) 
		pGuild = AllocObj(GuildData);

	pGuild->Decode(iPacket);
	User *pUser = nullptr;
	for (auto& nID : pGuild->anCharacterID)
	{
		if ((pUser = User::FindUser(nID)) != nullptr)
		{
			pUser->SetGuildName(pGuild->sGuildName);
			pUser->SetGuildMark(
				pGuild->nMarkBg,
				pGuild->nMarkBgColor,
				pGuild->nMark,
				pGuild->nMarkColor
			);
			m_mCharIDToGuildID[nID] = pGuild->nGuildID;
		}
	}
	m_mGuild[pGuild->nGuildID] = pGuild;
	m_mNameToGuild[pGuild->sGuildName] = pGuild;
	OutPacket oPacket;
	MakeGuildUpdatePacket(&oPacket, pGuild);
	Broadcast(&oPacket, pGuild->anCharacterID, 0);
}

void GuildMan::OnCreateNewGuildDone(InPacket * iPacket)
{
	int nCharacterID = iPacket->Decode4();
	int nRet = iPacket->Decode1();
	if (!nRet)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);

		auto pGuild = AllocObj(GuildData);
		pGuild->Decode(iPacket);
		for (int i = 0; i < pGuild->anCharacterID.size(); ++i) 
		{
			auto pUser = User::FindUser(pGuild->anCharacterID[i]);
			if (pUser)
			{
				m_mCharIDToGuildID[pGuild->anCharacterID[i]] = pGuild->nGuildID;
				pGuild->aMemberData[i].sCharacterName = pUser->GetName();
				pGuild->aMemberData[i].nJob = QWUser::GetJob(pUser);
				pGuild->aMemberData[i].nLevel = QWUser::GetLevel(pUser);
				pGuild->aMemberData[i].bOnline = true;
				pUser->SetGuildName(pGuild->sGuildName);
				pUser->SetGuildMark(
					pGuild->nMarkBg,
					pGuild->nMarkBgColor,
					pGuild->nMark,
					pGuild->nMarkColor
				);
			}
			else
				pGuild->aMemberData[i].bOnline = false;
		}
		m_mGuild[pGuild->nGuildID] = pGuild;
		OutPacket oPacket;
		MakeGuildUpdatePacket(&oPacket, pGuild);

		oPacket.GetSharedPacket()->ToggleBroadcasting();
		for (int i = 0; i < pGuild->anCharacterID.size(); ++i)
		{
			auto pUser = User::FindUser(pGuild->anCharacterID[i]);
			if (pUser) 
			{
				pUser->SendPacket(&oPacket);
			}
		}
	}
}

void GuildMan::OnGuildInviteRequest(User * pUser, InPacket * iPacket)
{
	std::string strTargetName = iPacket->DecodeStr();
	auto pTarget = User::FindUserByName(strTargetName);
	auto pGuild = GetGuildByCharID(pUser->GetUserID());
	auto nTargetGuildID = GetGuildIDByCharID(pTarget->GetUserID());

	if (pTarget && pGuild && nTargetGuildID == -1)
	{
		int nIdx = FindUser(pUser->GetUserID(), pGuild);

		//會長/副會長 = 1/2
		if (nIdx >= 0 && 
			pGuild->aMemberData[nIdx].nGrade <= 2 &&
			(int)pGuild->anCharacterID.size() < pGuild->nMaxMemberNum)
		{
			OutPacket oPacket;
			oPacket.Encode2(UserSendPacketFlag::UserLocal_OnGuildResult);
			oPacket.Encode1(GuildResult::res_Guild_Invite);
			oPacket.Encode4(pGuild->nGuildID);
			oPacket.EncodeStr(pUser->GetName());
			oPacket.Encode4(QWUser::GetLevel(pUser));
			oPacket.Encode4(QWUser::GetJob(pUser));

			pTarget->SendPacket(&oPacket);
			pTarget->AddGuildInvitedCharacterID(pUser->GetUserID());
		}
	}
}

void GuildMan::OnRemoveGuildRequest(User * pUser)
{
	auto pGuild = GetGuildByCharID(pUser->GetUserID());
	if (pGuild && IsGuildMaster(pGuild->nGuildID, pUser->GetUserID()))
	{
		OutPacket oPacket;
		oPacket.Encode2(GameSrvSendPacketFlag::GuildRequest);
		oPacket.Encode1(GuildRequest::rq_Guild_Withdraw_Kick);
		oPacket.Encode4(pGuild->nGuildID);
		oPacket.Encode4(pUser->GetUserID());
		oPacket.Encode4(pUser->GetUserID());
		oPacket.Encode1(0); //bKicked

		WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	}
}

void GuildMan::OnJoinGuildReqest(User * pUser, InPacket * iPacket)
{
	int nGuildID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	auto pGuild = GetGuild(nGuildID);
	if (nCharacterID == pUser->GetUserID() && 
		GetGuildIDByCharID(nCharacterID) == -1 &&
		pGuild &&
		(int)pGuild->anCharacterID.size() < pGuild->nMaxMemberNum)
	{
		auto sGuildInvitedChar = pUser->GetGuildInvitedCharacterID();
		OutPacket oPacket;
		oPacket.Encode2(GameSrvSendPacketFlag::GuildRequest);
		oPacket.Encode1(GuildRequest::rq_Guild_Join);
		oPacket.Encode4(nGuildID);
		oPacket.Encode4(nCharacterID);
		MemberData memberData;
		memberData.Set(
			pUser->GetName(),
			QWUser::GetLevel(pUser),
			QWUser::GetJob(pUser),
			true
		);
		memberData.Encode(&oPacket);
		oPacket.Encode1((char)sGuildInvitedChar.size());
		for (auto& nID : sGuildInvitedChar)
			oPacket.Encode4(nID);

		WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	}
}

void GuildMan::OnWithdrawGuildRequest(User * pUser, InPacket * iPacket)
{
	int nCharacterID = pUser->GetUserID();
	int nTargetID = iPacket->Decode4();
	std::string strTargetName = iPacket->DecodeStr();
	auto pGuild = GetGuildByCharID(pUser->GetUserID());
	if (pGuild && 
			//Withdraw by self                       //Is kicked, check the privilege
		((nCharacterID == nTargetID) || GetMemberGrade(pGuild->nGuildID, pUser->GetUserID()) <= 2))
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		auto nIdx = FindUser(nTargetID, pGuild);
		if(nIdx >= 0 && pGuild->aMemberData[nIdx].sCharacterName == strTargetName)
		{
			OutPacket oPacket;
			oPacket.Encode2(GameSrvSendPacketFlag::GuildRequest);
			oPacket.Encode1(GuildRequest::rq_Guild_Withdraw_Kick);
			oPacket.Encode4(pGuild->nGuildID);
			oPacket.Encode4(pUser->GetUserID());
			oPacket.Encode4(nTargetID);
			oPacket.Encode1(nCharacterID != nTargetID); //bKicked

			WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
		}
	}
}

void GuildMan::OnWithdrawGuildDone(InPacket * iPacket)
{
	int nMasterID = iPacket->Decode4();
	int nGuildID = iPacket->Decode4();
	User *pMaster = nullptr;

	auto pGuild = GetGuild(nGuildID);
	if (pGuild)
	{
		int nTargetID = iPacket->Decode4();
		auto pTarget = User::FindUser(nTargetID);
		std::string strTargetName = iPacket->DecodeStr();
		bool bKicked = iPacket->Decode1() == 1;

		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnGuildResult);

		//Remove Guild
		if (!bKicked && IsGuildMaster(nGuildID, nTargetID))
		{
			oPacket.Encode1(GuildResult::res_Guild_Send_Guild_DisbandSuccess_Dialog);
			oPacket.Encode4(nGuildID);
			oPacket.Encode1(1);
			Broadcast(&oPacket, pGuild->anCharacterID, 0);

			for (auto& nID : pGuild->anCharacterID) 
				RemoveUser(pGuild, nID);

			m_mGuild.erase(nGuildID);
			FreeObj(pGuild);
		}
		else
		{
			oPacket.Encode1(
				bKicked ? 
				GuildResult::res_Guild_Withdraw_Kicked : GuildResult::res_Guild_Withdraw
			);
			oPacket.Encode4(nGuildID);
			oPacket.Encode4(nTargetID);
			oPacket.EncodeStr(strTargetName);

			Broadcast(&oPacket, pGuild->anCharacterID, 0);
			RemoveUser(pGuild, nTargetID);
		}
	}
	else if ((pMaster = User::FindUser(nMasterID)))
	{
	}
}

void GuildMan::OnSetGradeNameRequest(User * pUser, InPacket * iPacket)
{
	auto pGuild = GetGuildByCharID(pUser->GetUserID());
	if (pGuild && (GetMemberGrade(pGuild->nGuildID, pUser->GetUserID()) <= 2))
	{
		OutPacket oPacket;
		oPacket.Encode2(GameSrvSendPacketFlag::GuildRequest);
		oPacket.Encode1(GuildRequest::rq_Guild_SetGradeName);
		oPacket.Encode4(pGuild->nGuildID);
		oPacket.Encode4(pUser->GetUserID());
		std::string strCheck = "";
		for (int i = 0; i < 5; ++i)
		{
			strCheck = iPacket->DecodeStr();
			if (strCheck.size() < 4 || strCheck.size() > 10)
				return;
			oPacket.EncodeStr(strCheck);
		}

		WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	}
}

void GuildMan::OnSetGradeNameDone(InPacket * iPacket)
{
	int nCharacterID = iPacket->Decode4();
	int nGuildID = iPacket->Decode4();
	auto pGuild = GetGuild(nGuildID);
	if (pGuild)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnGuildResult);
		oPacket.Encode1(GuildResult::res_Guild_SetGradeName);
		oPacket.Encode4(nGuildID);
		for (int i = 0; i < 5; ++i) 
		{
			pGuild->asGradeName[i] = iPacket->DecodeStr();
			oPacket.EncodeStr(pGuild->asGradeName[i]);
		}

		Broadcast(&oPacket, pGuild->anCharacterID, 0);
	}
	else
	{
		//Send an error message to pUser.
	}
}

void GuildMan::OnSetNoticeRequest(User * pUser, InPacket * iPacket)
{
	auto pGuild = GetGuildByCharID(pUser->GetUserID());
	if (pGuild && (GetMemberGrade(pGuild->nGuildID, pUser->GetUserID()) <= 2))
	{
		OutPacket oPacket;
		oPacket.Encode2(GameSrvSendPacketFlag::GuildRequest);
		oPacket.Encode1(GuildRequest::rq_Guild_SetNotice);
		oPacket.Encode4(pGuild->nGuildID);
		oPacket.Encode4(pUser->GetUserID());
		std::string strCheck = iPacket->DecodeStr();

		if (strCheck.size() > 100)
			return;
		oPacket.EncodeStr(strCheck);
		WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	}
}

void GuildMan::OnSetNoticeDone(InPacket * iPacket)
{
	int nCharacterID = iPacket->Decode4();
	int nGuildID = iPacket->Decode4();
	auto pGuild = GetGuild(nGuildID);
	if (pGuild)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		pGuild->sNotice = iPacket->DecodeStr();

		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnGuildResult);
		oPacket.Encode1(GuildResult::res_Guild_SetNotice);
		oPacket.Encode4(nGuildID);
		oPacket.EncodeStr(pGuild->sNotice);

		Broadcast(&oPacket, pGuild->anCharacterID, 0);
	}
	else
	{
		//Send an error message to pUser
	}
}

void GuildMan::OnSetMemberGradeRequest(User * pUser, InPacket * iPacket)
{
	int nTargetID = iPacket->Decode4();
	int nGrade = iPacket->Decode1();

	int nMasterGrade = -1, nTargetGrade = -1;
	auto pGuild = GetGuildByCharID(pUser->GetUserID());
	if (pGuild && (nGrade >= 2 && nGrade <= 5))
	{
		nMasterGrade = GetMemberGrade(pGuild->nGuildID, pUser->GetUserID());
		nTargetGrade = GetMemberGrade(pGuild->nGuildID, nTargetID);

		if (nMasterGrade > 2 || //Not even a SubMaster.
			nMasterGrade >= nTargetGrade || //Only the one with lower grade is able to set target's grade.
			(nMasterGrade == nGrade)) //Only the grade higher than self's is allowed.
			return;

		OutPacket oPacket;
		oPacket.Encode2(GameSrvSendPacketFlag::GuildRequest);
		oPacket.Encode1(GuildRequest::rq_Guild_SetMemberGrade);
		oPacket.Encode4(pGuild->nGuildID);
		oPacket.Encode4(pUser->GetUserID());
		oPacket.Encode4(nTargetID);
		oPacket.Encode1(nGrade);

		WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	}
}

void GuildMan::OnSetMemberGradeDone(InPacket * iPacket)
{
	int nMasterID = iPacket->Decode4();
	int nGuildID = iPacket->Decode4();
	auto pGuild = GetGuild(nGuildID);
	if (pGuild)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		int nTargetID = iPacket->Decode4();
		int nGrade = iPacket->Decode1();

		int nIdx = FindUser(nTargetID, pGuild);
		if (nIdx >= 0)
			pGuild->aMemberData[nIdx].nGrade = nGrade;
		
		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnGuildResult);
		oPacket.Encode1(GuildResult::res_Guild_SetMemberGrade);
		oPacket.Encode4(nGuildID);
		oPacket.Encode4(nTargetID);
		oPacket.Encode1((char)nGrade);

		Broadcast(&oPacket, pGuild->anCharacterID, 0);
	}
	else
	{
		//Send error message to pMaster.
	}
}

void GuildMan::OnAskGuildMark(User * pUser)
{
	auto pGuild = GetGuildByCharID(pUser->GetUserID());
	if (pGuild && IsGuildMaster(pGuild->nGuildID, pUser->GetUserID()))
	{
		OutPacket oPacket;
		oPacket.Encode2(GameSrvSendPacketFlag::GuildRequest);
		oPacket.Encode1(GuildResult::res_Guild_AskMark);
		pUser->SendPacket(&oPacket);
	}
}

void GuildMan::OnSetMarkRequest(User * pUser, InPacket * iPacket)
{
	auto pGuild = GetGuildByCharID(pUser->GetUserID());
	if (pGuild && IsGuildMaster(pGuild->nGuildID, pUser->GetUserID()))
	{
		OutPacket oPacket;
		oPacket.Encode2(GameSrvSendPacketFlag::GuildRequest);
		oPacket.Encode1(GuildRequest::rq_Guild_SetMark);
		oPacket.Encode4(pGuild->nGuildID);
		oPacket.Encode4(pUser->GetUserID());

		//Check if the marks are valid
		int nMarkBg = iPacket->Decode2();
		int nMarkBgColor = iPacket->Decode1();
		int nMark = iPacket->Decode2();
		int nMarkColor = iPacket->Decode1();

		oPacket.Encode2(nMarkBg);
		oPacket.Encode1(nMarkBgColor);
		oPacket.Encode2(nMark);
		oPacket.Encode1(nMarkColor);

		WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	}
}

void GuildMan::OnSetMarkDone(InPacket * iPacket)
{
	int nMasterID = iPacket->Decode4();
	int nGuildID = iPacket->Decode4();
	auto pGuild = GetGuild(nGuildID);
	if (pGuild)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);

		pGuild->nMarkBg = iPacket->Decode2();
		pGuild->nMarkBgColor = iPacket->Decode1();
		pGuild->nMark = iPacket->Decode2();
		pGuild->nMarkColor = iPacket->Decode1();

		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnGuildResult);
		oPacket.Encode1(GuildResult::res_Guild_SetMark);
		oPacket.Encode4(nGuildID);
		oPacket.Encode2(pGuild->nMarkBg);
		oPacket.Encode1(pGuild->nMarkBgColor);
		oPacket.Encode2(pGuild->nMark);
		oPacket.Encode1(pGuild->nMarkColor);

		Broadcast(&oPacket, pGuild->anCharacterID, 0);
		User *pUser = nullptr;
		for (auto& nID : pGuild->anCharacterID)
		{
			pUser = User::FindUser(nID);
			if(pUser)
				pUser->SetGuildMark(
					pGuild->nMarkBg,
					pGuild->nMarkBgColor,
					pGuild->nMark,
					pGuild->nMarkColor
				);
		}
	}
	else
	{
		//Send error message to pMaster.
	}
}

void GuildMan::OnCreateNewGuildRequest(User *pUser, const std::string &strGuildName, const std::vector<std::pair<int, MemberData>>& aMember)
{
	auto pGuild = GetGuildByCharID(pUser->GetUserID());
	if (!pGuild && !GetGuildByName(strGuildName))
	{
		OutPacket oPacket;
		oPacket.Encode2(GameSrvSendPacketFlag::GuildRequest);
		oPacket.Encode1(GuildRequest::rq_Guild_Create);
		oPacket.Encode4(pUser->GetUserID());
		oPacket.EncodeStr(strGuildName);
		oPacket.Encode1((char)aMember.size());
		for (auto& memberData : aMember) 
		{
			oPacket.Encode4(memberData.first);
			memberData.second.Encode(&oPacket);
		}

		WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	}
}

void GuildMan::OnJoinGuildDone(InPacket * iPacket)
{
	int nCharacterID = iPacket->Decode4();
	auto pUser = User::FindUser(nCharacterID);
	if (pUser)
		pUser->ClearGuildInvitedCharacterID();
	int nGuildID = iPacket->Decode4();
	auto pGuild = GetGuild(nGuildID);
	if (nGuildID != -1 && pGuild)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnGuildResult);
		oPacket.Encode1(GuildResult::res_Guild_Join);
		oPacket.Encode4(nGuildID);
		oPacket.Encode4(nCharacterID);
		MemberData memberData;
		memberData.Decode(iPacket);
		memberData.Encode(&oPacket);
		pGuild->anCharacterID.push_back(nCharacterID);
		pGuild->aMemberData.push_back(std::move(memberData));

		if (pUser)
		{
			m_mCharIDToGuildID[nCharacterID] = nGuildID;
			OutPacket oPacketForGuildUpdating;
			MakeGuildUpdatePacket(&oPacketForGuildUpdating, pGuild);
			pUser->SendPacket(&oPacketForGuildUpdating);
			pUser->SetGuildName(pGuild->sGuildName);
			pUser->SetGuildMark(
				pGuild->nMarkBg,
				pGuild->nMarkBgColor,
				pGuild->nMark,
				pGuild->nMarkColor
			);
		}
		Broadcast(&oPacket, pGuild->anCharacterID, 0);
	}
}

void GuildMan::OnNotifyLoginOrLogout(InPacket * iPacket)
{
	int nGuildID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	auto pGuild = GetGuild(nGuildID);
	if (pGuild)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		int nIdx = FindUser(nCharacterID, pGuild);
		if (nIdx >= 0)
		{
			pGuild->aMemberData[nIdx].bOnline = iPacket->Decode1() == 1;
			OutPacket oPacket;
			oPacket.Encode2(UserSendPacketFlag::UserLocal_OnGuildResult);
			oPacket.Encode1(GuildResult::res_Guild_Notify_LoginOrLogout);
			oPacket.Encode4(nGuildID);
			oPacket.Encode4(nCharacterID);
			oPacket.Encode1(pGuild->aMemberData[nIdx].bOnline);

			Broadcast(&oPacket, pGuild->anCharacterID, -nCharacterID);
		}
	}
}

void GuildMan::MakeGuildUpdatePacket(OutPacket * oPacket, GuildData * pGuild)
{
	oPacket->Encode2(UserSendPacketFlag::UserLocal_OnGuildResult);
	oPacket->Encode1(GuildResult::res_Guild_Update);
	oPacket->Encode1(1);
	pGuild->Encode(oPacket);
}
#endif

bool GuildMan::IsGuildMaster(int nGuildID, int nCharacterID)
{
	auto pGuild = GetGuild(nGuildID);
	auto nIdx = FindUser(nCharacterID, pGuild);
	if (nIdx >= 0)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		return pGuild->aMemberData[nIdx].nGrade == 1;
	}
	return false;
}

bool GuildMan::IsGuildSubMaster(int nGuildID, int nCharacterID)
{
	auto pGuild = GetGuild(nGuildID);
	auto nIdx = FindUser(nCharacterID, pGuild);
	if (nIdx >= 0)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		return pGuild->aMemberData[nIdx].nGrade == 2;
	}
	return false;
}

bool GuildMan::IsGuildMember(int nGuildID, int nCharacterID)
{
	auto pGuild = GetGuild(nGuildID);
	return FindUser(nCharacterID, pGuild) >= 0;
}

bool GuildMan::IsGuildFull(int nGuildID)
{
	auto pGuild = GetGuild(nGuildID);
	std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
	return (int)pGuild->aMemberData.size() == pGuild->nMaxMemberNum;
}

int GuildMan::GetMemberGrade(int nGuildID, int nCharacterID)
{
	auto pGuild = GetGuild(nGuildID);
	int nIdx = FindUser(nCharacterID, pGuild);
	std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
	return nIdx >= 0 ? pGuild->aMemberData[nIdx].nGrade : 0xFFFF;
}

void GuildMan::RemoveUser(GuildData *pGuild, int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
	int nIdx = FindUser(nCharacterID, pGuild);
	if (pGuild && nIdx >= 0)
	{
		pGuild->aMemberData.erase(
			pGuild->aMemberData.begin() + nIdx
		);
		pGuild->anCharacterID.erase(
			pGuild->anCharacterID.begin() + nIdx
		);
		m_mCharIDToGuildID.erase(nCharacterID);

#ifdef _WVSGAME
		auto pUser = User::FindUser(nCharacterID);
		if (pUser)
		{
			pUser->SetGuildName("");
			pUser->SetGuildMark(0, 0, 0, 0);
		}
#endif
	}
}

#ifdef _WVSCENTER
void GuildMan::SendToAll(GuildData * pGuild, OutPacket * oPacket)
{
	oPacket->GetSharedPacket()->ToggleBroadcasting();

	bool bSrvSent[30]{ 0 };
	for (auto& nID : pGuild->anCharacterID)
	{
		auto pUser = WvsWorld::GetInstance()->GetUser(nID);
		if (pUser)
		{
			auto pSrv = WvsBase::GetInstance<WvsCenter>()->GetChannel(pUser->m_nChannelID);
			if (pSrv && !bSrvSent[pUser->m_nChannelID]) 
			{
				bSrvSent[pUser->m_nChannelID] = true;
				pSrv->GetLocalSocket()->SendPacket(oPacket);
			}
		}
	}
}

void GuildMan::LoadGuild(InPacket *iPacket, OutPacket * oPacket)
{
	int nCharacterID = iPacket->Decode4();
	GuildData* pGuild = GetGuildByCharID(nCharacterID);
	oPacket->Encode2(CenterSendPacketFlag::GuildResult);
	oPacket->Encode1(GuildMan::GuildResult::res_Guild_Load);
	oPacket->Encode4(nCharacterID);

	std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);

	//Existed
	if (!pGuild && (pGuild = GetGuild(GuildDBAccessor::GetInstance()->LoadGuildID(nCharacterID))))
		m_mCharIDToGuildID[nCharacterID] = pGuild->nGuildID;

	//Load
	else if(!pGuild && (pGuild = (GuildData*)GuildDBAccessor::GetInstance()->LoadGuildByCharID(nCharacterID)), pGuild != nullptr)
	{
		m_mGuild[pGuild->nGuildID] = pGuild;
		m_mNameToGuild[pGuild->sGuildName] = pGuild;
		int nID = 0;
		for (int i = 0; i < pGuild->anCharacterID.size(); ++i) 
		{
			nID = pGuild->anCharacterID[i];
			auto pwUser = WvsWorld::GetInstance()->GetUser(nID);
			pGuild->aMemberData[i].bOnline = (pwUser != nullptr);
			if(pwUser)
				m_mCharIDToGuildID[nID] = pGuild->nGuildID;
		}
	}

	if (pGuild)
	{
		oPacket->Encode4(pGuild->nGuildID);
		pGuild->Encode(oPacket);
	}
	else
		oPacket->Encode4(-1);
}

void GuildMan::CreateNewGuild(InPacket *iPacket, OutPacket *oPacket)
{
	int nCharacterID = iPacket->Decode4();
	std::string strGuildName = iPacket->DecodeStr();
	int nMemberCount = iPacket->Decode1();

	std::vector<int> aMemberID;
	std::vector<MemberData> aMemberData;
	for (int i = 0; i < nMemberCount; ++i) 
	{
		aMemberID.push_back(iPacket->Decode4());
		MemberData memberData;
		memberData.Decode(iPacket);
		aMemberData.push_back(std::move(memberData));
	}

	oPacket->Encode2(CenterSendPacketFlag::GuildResult);
	oPacket->Encode1(GuildResult::res_Guild_Create);
	oPacket->Encode4(nCharacterID);

	std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
	if (!GetGuildByName(strGuildName))
	{
		GuildData *pGuild = AllocObj(GuildData);
		for (int i = 0; i < nMemberCount; ++i)
		{
			aMemberData[i].nGrade = (i == 0 ? 1 : 3);
			if (GetGuildIDByCharID(aMemberID[i]) >= 0) //Failed
			{
				oPacket->Encode1(1); //Failed;
				return;
			}
		}

		pGuild->nGuildID = ++m_atiGuildIDCounter;
		for (auto& nID : aMemberID)
			if(WvsWorld::GetInstance()->GetUser(nID))
				m_mCharIDToGuildID[nID] = pGuild->nGuildID;

		pGuild->anCharacterID = std::move(aMemberID);
		pGuild->aMemberData = std::move(aMemberData);
		pGuild->sGuildName = strGuildName;
		pGuild->nLoggedInUserCount = nMemberCount;
		pGuild->asGradeName.push_back("會長");
		pGuild->asGradeName.push_back("副會長");
		pGuild->asGradeName.push_back("公會成員");
		pGuild->asGradeName.push_back("公會成員");
		pGuild->asGradeName.push_back("公會成員");
		pGuild->nMaxMemberNum = 10;

		m_mNameToGuild[strGuildName] = pGuild;
		m_mGuild[pGuild->nGuildID] = pGuild;

		oPacket->Encode1(0);
		pGuild->Encode(oPacket);
		GuildDBAccessor::GetInstance()->CreateNewGuild(
			pGuild,
			WvsWorld::GetInstance()->GetWorldInfo().nWorldID);
	}
}

void GuildMan::JoinGuild(InPacket * iPacket, OutPacket * oPacket)
{
	int nGuildID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	auto pGuild = GetGuild(nGuildID);

	oPacket->Encode2(CenterSendPacketFlag::GuildResult);
	oPacket->Encode1(GuildResult::res_Guild_Join);
	oPacket->Encode4(nCharacterID);
	
	if (GetGuildIDByCharID(nCharacterID) == -1 &&
		pGuild &&
		(int)pGuild->anCharacterID.size() < pGuild->nMaxMemberNum &&
		WvsWorld::GetInstance()->GetUser(nCharacterID))
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		MemberData memberData;
		memberData.Decode(iPacket);

		int nInviterCount = iPacket->Decode1(), nInviter = 0, nIdx = -1;
		bool bValid = false;

		for (int i = 0; i < nInviterCount; ++i)
		{
			nInviter = iPacket->Decode4();
			if (GetGuildIDByCharID(nInviter) == pGuild->nGuildID &&
				((nIdx = FindUser(nInviter, pGuild), nIdx >= 0)) &&
				(pGuild->aMemberData[nIdx].nGrade <= 2) &&
					WvsWorld::GetInstance()->GetUser(nInviter))
			{
				bValid = true;
				break;
			}
		}
		if (bValid)
		{
			memberData.nGrade = 5;
			oPacket->Encode4(pGuild->nGuildID);
			memberData.Encode(oPacket);

			GuildDBAccessor::GetInstance()->JoinGuild(
				&memberData,
				nCharacterID,
				nGuildID,
				WvsWorld::GetInstance()->GetWorldInfo().nWorldID
 			);

			pGuild->aMemberData.push_back(std::move(memberData));
			pGuild->anCharacterID.push_back(nCharacterID);
			SendToAll(pGuild, oPacket);
			oPacket->Reset();
		}
		else
			oPacket->Encode4(-1);
	}
	else
		oPacket->Encode4(-1);
}

void GuildMan::WithdrawGuild(InPacket * iPacket, OutPacket * oPacket)
{
	int nGuildID = iPacket->Decode4();
	int nMasterID = iPacket->Decode4();
	int nTargetID = iPacket->Decode4();
	bool bKicked = iPacket->Decode1() == 1;
	auto pGuild = GetGuild(nGuildID);

	oPacket->Encode2(CenterSendPacketFlag::GuildResult);
	oPacket->Encode1(GuildResult::res_Guild_Withdraw);
	oPacket->Encode4(nMasterID);
	if (pGuild)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		int nMasterIdx = FindUser(nMasterID, pGuild);
		int nTargetIdx = FindUser(nTargetID, pGuild);
		int nMasterGrade = GetMemberGrade(nGuildID, nMasterID);
		if (nMasterIdx >= 0 &&
			nTargetIdx >= 0 &&
			GuildDBAccessor::GetInstance()->LoadGuildID(nTargetID) == nGuildID && //Check Again
			WvsWorld::GetInstance()->GetUser(nMasterID) &&
		 //Withdraw by self
			(!bKicked || (nMasterGrade <= 2 && (GetMemberGrade(nGuildID, nTargetID) > nMasterGrade))))
		{
			oPacket->Encode4(nGuildID);
			oPacket->Encode4(nTargetID);
			oPacket->EncodeStr(pGuild->aMemberData[nTargetIdx].sCharacterName);
			oPacket->Encode1((char)bKicked);
			SendToAll(pGuild, oPacket);
			oPacket->Reset();
			if (!bKicked && IsGuildMaster(nGuildID, nTargetID))
			{
				for (auto& nID : pGuild->anCharacterID)
				{
					GuildDBAccessor::GetInstance()->WithdrawGuild(
						nID,
						nGuildID,
						WvsWorld::GetInstance()->GetWorldInfo().nWorldID
					);
				}
				for (auto& nID : pGuild->anCharacterID)
					RemoveUser(pGuild, nID);

				m_mGuild.erase(nGuildID);
				GuildDBAccessor::GetInstance()->RemoveGuild(
					nGuildID, 
					WvsWorld::GetInstance()->GetWorldInfo().nWorldID
				);
				FreeObj(pGuild);
			}
			else
			{
				RemoveUser(pGuild, nTargetID);
				GuildDBAccessor::GetInstance()->WithdrawGuild(
					nTargetID,
					nGuildID,
					WvsWorld::GetInstance()->GetWorldInfo().nWorldID
				);
			}
		}
		else
			oPacket->Encode4(-1);
	}
}

void GuildMan::SetNotice(InPacket * iPacket, OutPacket * oPacket)
{
	int nGuildID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	auto pGuild = GetGuild(nGuildID);
	oPacket->Encode2(CenterSendPacketFlag::GuildResult);
	oPacket->Encode1(GuildResult::res_Guild_SetNotice);
	oPacket->Encode4(nCharacterID);

	if (pGuild && GetMemberGrade(nGuildID, nCharacterID) <= 2)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		oPacket->Encode4(nGuildID);

		std::string strCheck = iPacket->DecodeStr();
		if (strCheck.size() > 100)
			return;

		pGuild->sNotice = strCheck;
		oPacket->EncodeStr(strCheck);
		GuildDBAccessor::GetInstance()->UpdateGuild(
			pGuild, WvsWorld::GetInstance()->GetWorldInfo().nWorldID
		);
		SendToAll(pGuild, oPacket);
		oPacket->Reset();
	}
	else
		oPacket->Encode4(-1);
}

void GuildMan::SetGradeName(InPacket * iPacket, OutPacket * oPacket)
{
	int nGuildID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	auto pGuild = GetGuild(nGuildID);
	oPacket->Encode2(CenterSendPacketFlag::GuildResult);
	oPacket->Encode1(GuildResult::res_Guild_SetGradeName);
	oPacket->Encode4(nCharacterID);

	if (pGuild && GetMemberGrade(nGuildID, nCharacterID) <= 2)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		std::string strCheck = "";
		oPacket->Encode4(nGuildID);

		for (int i = 0; i < 5; ++i)
		{
			strCheck = iPacket->DecodeStr();
			if (strCheck.size() < 4 || strCheck.size() > 10)
				return;
			pGuild->asGradeName[i] = strCheck;
			oPacket->EncodeStr(strCheck);
		}

		GuildDBAccessor::GetInstance()->UpdateGuild(
			pGuild, WvsWorld::GetInstance()->GetWorldInfo().nWorldID
		);
		SendToAll(pGuild, oPacket);
		oPacket->Reset();
	}
	else
		oPacket->Encode4(-1);
}

void GuildMan::SetMemberGrade(InPacket * iPacket, OutPacket * oPacket)
{
	int nGuildID = iPacket->Decode4();
	auto pGuild = GetGuild(nGuildID);
	int nMasterID = iPacket->Decode4();
	int nTargetID = iPacket->Decode4();
	int nGrade = iPacket->Decode1();

	oPacket->Encode2(CenterSendPacketFlag::GuildResult);
	oPacket->Encode1(GuildResult::res_Guild_SetMemberGrade);
	oPacket->Encode4(nMasterID);

	if (pGuild && (nGrade >= 2 && nGrade <= 5))
	{
		int nMasterGrade = GetMemberGrade(pGuild->nGuildID, nMasterID);
		int nTargetGrade = GetMemberGrade(pGuild->nGuildID, nTargetID);

		if (nMasterGrade > 2 || //Not even a SubMaster.
			nMasterGrade >= nTargetGrade || //Only the one with lower grade is able to set target's grade.
			(nMasterGrade == nGrade)) //Only the grade higher than self's is allowed.
		{
			oPacket->Encode4(-1);
			return;
		}

		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		int nIdx = FindUser(nTargetID, pGuild);
		pGuild->aMemberData[nIdx].nGrade = nGrade;

		GuildDBAccessor::GetInstance()->UpdateGuildMember(
			&(pGuild->aMemberData[nIdx]),
			nTargetID,
			nGuildID,
			WvsWorld::GetInstance()->GetWorldInfo().nWorldID
		);
		oPacket->Encode4(pGuild->nGuildID);
		oPacket->Encode4(nTargetID);
		oPacket->Encode1(nGrade);
		SendToAll(pGuild, oPacket);
		oPacket->Reset();
	}
}

void GuildMan::SetMark(InPacket * iPacket, OutPacket * oPacket)
{
	int nGuildID = iPacket->Decode4();
	int nMasterID = iPacket->Decode4();

	oPacket->Encode2(CenterSendPacketFlag::GuildResult);
	oPacket->Encode1(GuildResult::res_Guild_SetMark);
	oPacket->Encode4(nMasterID);

	auto pGuild = GetGuild(nGuildID);
	if (pGuild && IsGuildMaster(nGuildID, nMasterID))
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		pGuild->nMarkBg = iPacket->Decode2();
		pGuild->nMarkBgColor = iPacket->Decode1();
		pGuild->nMark = iPacket->Decode2();
		pGuild->nMarkColor = iPacket->Decode1();

		oPacket->Encode4(nGuildID);
		oPacket->Encode2(pGuild->nMarkBg);
		oPacket->Encode1(pGuild->nMarkBgColor);
		oPacket->Encode2(pGuild->nMark);
		oPacket->Encode1(pGuild->nMarkColor);

		GuildDBAccessor::GetInstance()->UpdateGuild(
			pGuild, WvsWorld::GetInstance()->GetWorldInfo().nWorldID
		);
		SendToAll(pGuild, oPacket);
		oPacket->Reset();
	}
	else
		oPacket->Encode4(-1);
}

void GuildMan::IncMaxMemberNum(InPacket * iPacket, OutPacket * oPacket)
{
	int nGuildID = iPacket->Decode4();
	int nMasterID = iPacket->Decode4();
	oPacket->Encode2(CenterSendPacketFlag::GuildResult);
	oPacket->Encode1(GuildResult::res_Guild_IncMaxMemberNum);
	oPacket->Encode4(nMasterID);
	
	auto pGuild = GetGuild(nGuildID);
	if (pGuild && IsGuildMaster(nGuildID, nMasterID))
	{
		int nToInc = iPacket->Decode4();
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		pGuild->nMaxMemberNum += nToInc;

		GuildDBAccessor::GetInstance()->UpdateGuild(
			pGuild, WvsWorld::GetInstance()->GetWorldInfo().nWorldID
		);

		oPacket->Encode4(nGuildID);
		oPacket->Encode1(pGuild->nMaxMemberNum);
		SendToAll(pGuild, oPacket);
		oPacket->Reset();
	}
	else
		oPacket->Encode4(-1);
}

void GuildMan::IncPoint(InPacket * iPacket, OutPacket * oPacket)
{
	int nGuildID = iPacket->Decode4();
	oPacket->Encode2(CenterSendPacketFlag::GuildResult);
	oPacket->Encode1(GuildResult::res_Guild_IncPoint);

	auto pGuild = GetGuild(nGuildID);
	if (pGuild)
	{
		int nToInc = iPacket->Decode4();
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		pGuild->nPoint += nToInc;

		GuildDBAccessor::GetInstance()->UpdateGuild(
			pGuild, WvsWorld::GetInstance()->GetWorldInfo().nWorldID
		);

		oPacket->Encode4(nGuildID);
		oPacket->Encode1(pGuild->nMaxMemberNum);
		SendToAll(pGuild, oPacket);
		oPacket->Reset();
	}
	else
		oPacket->Encode4(-1);
}

void GuildMan::NotifyLoginOrLogout(int nCharacterID, bool bMigrateIn)
{
	auto pGuild = GetGuildByCharID(nCharacterID);
	int nGuildID = -1;

	//Guild not exists or not yet been loaded.
	if (!pGuild && !(pGuild = GetGuild(GuildDBAccessor::GetInstance()->LoadGuildID(nCharacterID))))
		pGuild = (GuildData*)GuildDBAccessor::GetInstance()->LoadGuildByCharID(nCharacterID);

	//If the guild exists and is loaded, then notify all members.
	if (pGuild)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxGuildLock);
		auto nIdx = FindUser(nCharacterID, pGuild);
		bool bChangeChannel = pGuild->aMemberData[nIdx].bOnline && bMigrateIn;

		if (nIdx >= 0)
			pGuild->aMemberData[nIdx].bOnline = bMigrateIn;
		if (!bMigrateIn)
			m_mCharIDToGuildID.erase(nCharacterID);
		else
			m_mCharIDToGuildID[nCharacterID] = pGuild->nGuildID;

		if (!bChangeChannel)
		{
			OutPacket oPacket;
			oPacket.Encode2(CenterSendPacketFlag::GuildResult);
			oPacket.Encode1(GuildResult::res_Guild_Notify_LoginOrLogout);
			oPacket.Encode4(pGuild->nGuildID);
			oPacket.Encode4(nCharacterID);
			oPacket.Encode1(bMigrateIn);

			SendToAll(pGuild, &oPacket);
		}
	}
}
#endif

void GuildMan::MemberData::Set(const std::string & strName, int nLevel, int nJob, bool bOnline)
{
	sCharacterName = strName;
	this->nLevel = nLevel;
	this->nJob = nJob;
	this->bOnline = bOnline;
}

void GuildMan::MemberData::Encode(OutPacket * oPacket) const
{
	oPacket->EncodeBuffer((unsigned char*)sCharacterName.c_str(), 13);
	oPacket->Encode4(nJob);
	oPacket->Encode4(nLevel);
	oPacket->Encode4(nGrade);
	oPacket->Encode4(bOnline ? 1 : 0);
	oPacket->Encode4(nContribution);
}

void GuildMan::MemberData::Decode(InPacket * iPacket)
{
	char strBuffer[14]{ 0 };
	iPacket->DecodeBuffer((unsigned char*)strBuffer, 13);
	sCharacterName = strBuffer;

	nJob = iPacket->Decode4();
	nLevel = iPacket->Decode4();
	nGrade = iPacket->Decode4();
	bOnline = iPacket->Decode4() == 1;
	nContribution = iPacket->Decode4();
}

void GuildMan::GuildData::Encode(OutPacket * oPacket) const
{
	oPacket->Encode4(nGuildID);
	oPacket->EncodeStr(sGuildName);

	for (int i = 0; i < 5; ++i)
		oPacket->EncodeStr(asGradeName[i]);

	oPacket->Encode1((char)aMemberData.size());
	oPacket->EncodeBuffer((unsigned char*)anCharacterID.data(), sizeof(int) * ((int)aMemberData.size()));
	for (auto& memberData : aMemberData)
		memberData.Encode(oPacket);

	oPacket->Encode4(nMaxMemberNum);
	oPacket->Encode2(nMarkBg);
	oPacket->Encode1(nMarkBgColor);
	oPacket->Encode2(nMark);
	oPacket->Encode1(nMarkColor);
	oPacket->EncodeStr(sNotice);
	oPacket->Encode4(nPoint);
}

void GuildMan::GuildData::Decode(InPacket * iPacket)
{
	nGuildID = iPacket->Decode4();
	sGuildName = iPacket->DecodeStr();

	asGradeName.clear();
	for (int i = 0; i < 5; ++i)
		asGradeName.push_back(iPacket->DecodeStr());

	int nMemberCount = iPacket->Decode1();
	anCharacterID.clear();
	for (int i = 0; i < nMemberCount; ++i)
		anCharacterID.push_back(iPacket->Decode4());

	aMemberData.clear();
	for (int i = 0; i < nMemberCount; ++i) 
	{
		MemberData memberData;
		memberData.Decode(iPacket);
		aMemberData.push_back(std::move(memberData));
	}

	nMaxMemberNum = iPacket->Decode4();
	nMarkBg = iPacket->Decode2();
	nMarkBgColor = iPacket->Decode1();
	nMark = iPacket->Decode2();
	nMarkColor = iPacket->Decode1();
	sNotice = iPacket->DecodeStr();
	nPoint = iPacket->Decode4();
}
