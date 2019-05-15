#include "MiniRoomBase.h"
#include "User.h"
#include "TradingRoom.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Net\InPacket.h"

std::atomic<int> MiniRoomBase::ms_nSNCounter = 1;

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
}

MiniRoomBase::~MiniRoomBase()
{
}

int MiniRoomBase::GetType()
{
	return m_nType;
}

void MiniRoomBase::OnPacketBase(int nType, User *pUser, InPacket *iPacket)
{
}

void MiniRoomBase::OnCreateBase(User *pUser, InPacket *iPacket, int nRound)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	if (pUser->CanAttachAdditionalProcess())
	{
		pUser->SetMiniRoom(this);
	}
}

MiniRoomBase* MiniRoomBase::Create(User *pUser, int nMiniRoomType, InPacket *iPacket, bool bTournament, int nRound)
{
	auto pMiniRoom = MiniRoomFactory(nMiniRoomType, iPacket, bTournament);
	if (!pMiniRoom) 
		return nullptr; //Error.
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
	}
	else if (nMiniRoomType == MiniRoomType::e_MiniRoom_EntrustedShop)
	{
	}

	return pRet;
}
