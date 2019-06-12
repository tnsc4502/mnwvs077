#include "UMiniRoom.h"
#include "MiniRoomBase.h"
#include "User.h"
#include "..\WvsLib\Net\InPacket.h"

void UMiniRoom::OnMiniRoom(User *pUser, InPacket *iPacket)
{
	int nType = iPacket->Decode1();
	if (nType)
	{
		switch (nType)
		{
			case MiniRoomBase::MiniRoomRequest::rq_MiniRoom_MRInviteResult:
				OnMRInviteResult(pUser, iPacket);
				break;
			case MiniRoomBase::MiniRoomRequest::rq_MiniRoom_MREnter:
				OnMREnter(pUser, iPacket);
				break;
			default:
				OnMRForward(pUser, nType, iPacket);
				break;
		}
	}
	else
		OnMRCreate(pUser, iPacket);
}

void UMiniRoom::OnMRCreate(User *pUser, InPacket *iPacket)
{
	int nMiniRoomType = iPacket->Decode1();
	if (nMiniRoomType &&
		pUser->GetField() &&
		(nMiniRoomType != MiniRoomBase::MiniRoomType::e_MiniRoom_EntrustedShop ||
			!pUser->HasOpenedEntrustedShop())) 
	{
		MiniRoomBase::Create(pUser, nMiniRoomType, iPacket, false, 0);
	}
}

void UMiniRoom::OnMREnter(User *pUser, InPacket *iPacket)
{
	int nSN = iPacket->Decode4();
	if (nSN)
		MiniRoomBase::Enter(pUser, nSN, iPacket, false);
}

void UMiniRoom::OnMRInviteResult(User *pUser, InPacket *iPacket)
{
	int nSN = iPacket->Decode4();
	if (nSN)
		MiniRoomBase::InviteResult(pUser, nSN, iPacket->Decode1());
}

void UMiniRoom::OnMRForward(User *pUser, int nType, InPacket *iPacket)
{
	if (pUser->GetMiniRoom())
		pUser->GetMiniRoom()->OnPacketBase(pUser, nType, iPacket);
}
