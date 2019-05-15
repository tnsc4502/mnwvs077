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
				break;
			case MiniRoomBase::MiniRoomRequest::rq_MiniRoom_MREnter:
				break;
			default:
				OnMRForward(pUser, iPacket);
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
		MiniRoomBase::rq_MiniRoom_MRCreate(pUser, nMiniRoomType, iPacket, false, 0);
}

void UMiniRoom::OnMRForward(User *pUser, InPacket *iPacket)
{
}
