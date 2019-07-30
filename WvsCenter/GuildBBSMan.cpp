#include "GuildBBSMan.h"
#include "WvsWorld.h"
#include "..\Database\GuildBBSDBAccessor.h"
#include "..\WvsGame\GuildMan.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"
#include "..\WvsLib\Memory\ZMemory.h"
#include <algorithm>

GuildBBSMan::GuildBBSMan()
{
}

GuildBBSMan::~GuildBBSMan()
{
}

GuildBBSMan* GuildBBSMan::GetInstance()
{
	static GuildBBSMan* pInstance = new GuildBBSMan;
	return pInstance;
}

void GuildBBSMan::OnGuildBBSRequest(void *pGuild_, InPacket *iPacket)
{
	int nCharacterID = iPacket->Decode4();
	int nType = iPacket->Decode1();
	switch (nType)
	{
	case GuildBBSRequest::rq_GuildBBS_LoadList:
		LoadList(pGuild_, nCharacterID, iPacket);
		break;
	case GuildBBSRequest::rq_GuildBBS_ViewEntry:
		ViewEntry(pGuild_, nCharacterID, iPacket->Decode4());
		break;
	case GuildBBSRequest::rq_GuildBBS_RegisterEntry:
		RegisterEntry(pGuild_, nCharacterID, iPacket);
		break;
	case GuildBBSRequest::rq_GuildBBS_DeleteEntry:
		DeleteEntry(pGuild_, nCharacterID, iPacket);
		break;
	case GuildBBSRequest::rq_GuildBBS_RegisterComment:
		RegisterComment(pGuild_, nCharacterID, iPacket);
		break;
	case GuildBBSRequest::rq_GuildBBS_DeleteComment:
		DeleteComment(pGuild_, nCharacterID, iPacket);
		break;
	}
}

void GuildBBSMan::RegisterEntry(void *pGuild_, int nCharacterID, InPacket * iPacket)
{
	auto pGuild = (GuildMan::GuildData*)pGuild_;
	int nEntryID = -1;
	bool bUpdate = iPacket->Decode1() ? 1 : 0;
	bool bNotice = bUpdate ? false : (iPacket->Decode1() ? 1 : 0);
	if (bNotice &&
		GuildBBSDBAccessor::IsNoticeExist(
			WvsWorld::GetInstance()->GetWorldInfo().nWorldID,
			pGuild->nGuildID)
		)
		return;
	if (bUpdate)
	{
		nEntryID = iPacket->Decode4();
		iPacket->Decode1(); //Unk
	}
	std::string sTitle = iPacket->DecodeStr();
	std::string sText = iPacket->DecodeStr();
	int nEmoticon = iPacket->Decode4(),
		nWorldID = WvsWorld::GetInstance()->GetWorldInfo().nWorldID;

	if (bUpdate)
		GuildBBSDBAccessor::ModifyEntry(
			nWorldID,
			pGuild->nGuildID,
			nEntryID,
			nCharacterID,
			sTitle,
			sText,
			nEmoticon,
			GuildMan::GetInstance()->GetMemberGrade(pGuild->nGuildID, nCharacterID) <= 2
		);
	else if(GuildBBSDBAccessor::QueryEntryCount(nWorldID, pGuild->nGuildID) < 100)
		nEntryID = GuildBBSDBAccessor::RegisterEntry(
			nWorldID,
			pGuild->nGuildID,
			nCharacterID,
			sTitle,
			sText,
			nEmoticon,
			bNotice
		);
	ViewEntry(pGuild, nCharacterID, nEntryID);
}

void GuildBBSMan::DeleteEntry(void *pGuild_, int nCharacterID, InPacket *iPacket)
{
	auto pGuild = (GuildMan::GuildData*)pGuild_;
	int nEntryID = iPacket->Decode4();
	GuildBBSDBAccessor::DeleteEntry(
		WvsWorld::GetInstance()->GetWorldInfo().nWorldID,
		pGuild->nGuildID,
		nEntryID,
		nCharacterID,
		GuildMan::GetInstance()->GetMemberGrade(pGuild->nGuildID, nCharacterID) <= 2
	);
}

void GuildBBSMan::LoadList(void *pGuild_, int nCharacterID, InPacket *iPacket)
{
	auto pGuild = (GuildMan::GuildData*)pGuild_;
	int nParam = iPacket->Decode1();
	auto aRet = GuildBBSDBAccessor::LoadList(
		WvsWorld::GetInstance()->GetWorldInfo().nWorldID,
		pGuild->nGuildID
	);
	OutPacket oPacket;
	oPacket.Encode2(CenterSendPacketFlag::GuildBBSResult);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode1(GuildBBSResult::res_GuildBBS_LoadEntry);
	if (aRet.size() == 0)
	{
		oPacket.Encode1(0);
		oPacket.Encode4(0);
		oPacket.Encode4(0);
	}
	else
	{
		ZUniquePtr<BBSEntry> pEntry;
		bool bNoticeContained = ((BBSEntry*)aRet[0])->m_bIsNotice;
		oPacket.Encode1(bNoticeContained);
		if (bNoticeContained)
		{
			pEntry.reset((BBSEntry*)aRet[0]);
			pEntry->Encode(&oPacket);
			aRet.erase(aRet.begin());
		}
		int nEntryListStart = 10 * nParam;
		if (nEntryListStart >= (int)aRet.size())
			return;
		int nEntryListCount = std::min(10, (int)((int)aRet.size() - nEntryListStart));
		oPacket.Encode4((int)aRet.size() + (bNoticeContained ? 1 : 0));
		oPacket.Encode4(nEntryListCount);
		for (int i = 0; i < nEntryListCount; ++i)
		{
			pEntry.reset((BBSEntry*)aRet[nEntryListStart + i]);
			pEntry->Encode(&oPacket);
		}
	}
	auto pwUser = WvsWorld::GetInstance()->GetUser(nCharacterID);
	if (pwUser)
		pwUser->SendPacket(&oPacket);
}

void GuildBBSMan::ViewEntry(void *pGuild_, int nCharacterID, int nEntryID)
{
	auto pGuild = (GuildMan::GuildData*)pGuild_;
	ZUniquePtr<BBSEntry> pEntry = (BBSEntry*)GuildBBSDBAccessor::LoadEntryView(
		WvsWorld::GetInstance()->GetWorldInfo().nWorldID,
		pGuild->nGuildID,
		nEntryID
	);
	if (!pEntry)
		return;

	OutPacket oPacket;
	oPacket.Encode2(CenterSendPacketFlag::GuildBBSResult);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode1(GuildBBSResult::res_GuildBBS_ViewEntry);
	oPacket.Encode4(pEntry->m_nEntryID);
	oPacket.Encode4(pEntry->m_nCharacterID);
	oPacket.Encode8(pEntry->m_liDate);
	oPacket.EncodeStr(pEntry->m_sTitle);
	oPacket.EncodeStr(pEntry->m_sText);
	oPacket.Encode4(pEntry->m_nEmoticon);
	oPacket.Encode4((int)pEntry->m_aComment.size());
	for (auto& comment : pEntry->m_aComment)
	{
		oPacket.Encode4(comment.m_nSN);
		oPacket.Encode4(comment.m_nCharacterID);
		oPacket.Encode8(comment.m_liDate);
		oPacket.EncodeStr(comment.m_sComment);
	}

	auto pwUser = WvsWorld::GetInstance()->GetUser(nCharacterID);
	if (pwUser)
		pwUser->SendPacket(&oPacket);
}

void GuildBBSMan::RegisterComment(void *pGuild_, int nCharacterID, InPacket *iPacket)
{
	auto pGuild = (GuildMan::GuildData*)pGuild_;
	int nEntryID = iPacket->Decode4();
	std::string sComment = iPacket->DecodeStr();
	int nWorldID = WvsWorld::GetInstance()->GetWorldInfo().nWorldID;
	if(GuildBBSDBAccessor::QueryCommentCount(nWorldID, pGuild->nGuildID, nEntryID) < 20)
		GuildBBSDBAccessor::RegisterComment(
			nWorldID,
			pGuild->nGuildID,
			nEntryID,
			nCharacterID,
			sComment
		);
	ViewEntry(pGuild, nCharacterID, nEntryID);
}

void GuildBBSMan::DeleteComment(void *pGuild_, int nCharacterID, InPacket *iPacket)
{
	auto pGuild = (GuildMan::GuildData*)pGuild_;
	int nEntryID = iPacket->Decode4();
	int nCommentSN = iPacket->Decode4();

	GuildBBSDBAccessor::DeleteComment(
		WvsWorld::GetInstance()->GetWorldInfo().nWorldID,
		pGuild->nGuildID,
		nEntryID,
		nCharacterID,
		nCommentSN,
		GuildMan::GetInstance()->GetMemberGrade(pGuild->nGuildID, nCharacterID) <= 2
	);
	ViewEntry(pGuild, nCharacterID, nEntryID);
}

void GuildBBSMan::BBSEntry::Encode(OutPacket * oPacket)
{
	oPacket->Encode4(m_nEntryID);
	oPacket->Encode4(m_nCharacterID);
	oPacket->EncodeStr(m_sTitle);
	oPacket->Encode8(m_liDate);
	oPacket->Encode4(m_nEmoticon);
	oPacket->Encode4(m_nCommentCount);
}