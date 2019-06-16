#include "Pet.h"
#include "Field.h"
#include "User.h"
#include "MovePath.h"
#include "WvsPhysicalSpace2D.h"
#include "StaticFoothold.h"
#include "DropPool.h"
#include "..\Database\GW_ItemSlotPet.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"

Pet::Pet(ZSharedPtr<GW_ItemSlotBase>& pPetSlot)
	: m_pPetSlot(pPetSlot)
{
	SetMoveAction(0);
}

Pet::~Pet()
{
}

void Pet::SetIndex(unsigned char nIndex)
{
	m_nIndex = nIndex;
}

unsigned char Pet::GetIndex() const
{
	return m_nIndex;
}

void Pet::Init(User * pUser)
{
	m_pOwner = pUser;
	
	this->m_ptPos.x = pUser->GetPosX();
	this->m_ptPos.y = pUser->GetPosY() - 20;
	OnEnterField(pUser->GetField());
}

void Pet::OnPacket(InPacket *iPacket, int nType)
{
	switch (nType)
	{
		case UserRecvPacketFlag::User_OnPetMove:
			OnMove(iPacket);
			break;
		case UserRecvPacketFlag::User_OnPetActionSpeak:
		case UserRecvPacketFlag::User_OnPetAction:
			break;
		case UserRecvPacketFlag::User_OnPetDropPickupRequest:
			m_pField->GetDropPool()->OnPickUpRequest(m_pOwner, iPacket, this);
			break;
	}
}

void Pet::OnEnterField(Field * pField)
{
	m_pField = pField;
	this->m_ptPos.x = m_pOwner->GetPosX();
	this->m_ptPos.y = m_pOwner->GetPosY() - 20;
	int nNull = 0;
	auto pFH = pField->GetSpace2D()->GetFootholdUnderneath(
		m_ptPos.x,
		m_ptPos.y,
		&nNull
	);
	if (pFH)
		SetFh(pFH->GetSN());
	OutPacket oPacket;
	MakeEnterFieldPacket(&oPacket);
	pField->BroadcastPacket(&oPacket);
}

void Pet::OnLeaveField()
{
	OutPacket oPacket;
	MakeLeaveFieldPacket(&oPacket);
	m_pField->SplitSendPacket(&oPacket, nullptr);
}

void Pet::OnMove(InPacket *iPacket)
{
	MovePath movePath;
	movePath.Decode(iPacket);
	ValidateMovePath(&movePath);
	OutPacket oPacket;
	oPacket.Encode2((short)UserSendPacketFlag::UserCommon_Pet_OnMove);
	oPacket.Encode4(m_pOwner->GetUserID());
	oPacket.Encode1(m_nIndex);
	movePath.Encode(&oPacket);

	m_pField->SplitSendPacket(&oPacket, m_pOwner);
}

void Pet::MakeEnterFieldPacket(OutPacket *oPacket)
{
	oPacket->Encode2((short)UserSendPacketFlag::UserCommon_Pet_OnMakeEnterFieldPacket);
	oPacket->Encode4(m_pOwner->GetUserID());
	oPacket->Encode1(m_nIndex);
	oPacket->Encode1(1);
	EncodeInitData(oPacket);
}

void Pet::MakeLeaveFieldPacket(OutPacket *oPacket)
{
	oPacket->Encode2((short)UserSendPacketFlag::UserCommon_Pet_OnMakeEnterFieldPacket);
	oPacket->Encode4(m_pOwner->GetUserID());
	oPacket->Encode1(m_nIndex);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
}

void Pet::EncodeInitData(OutPacket *oPacket)
{
	oPacket->Encode1(1);
	oPacket->Encode4(m_pPetSlot->nItemID);
	oPacket->EncodeStr(GetItemSlot()->strPetName);
	oPacket->Encode8(m_pPetSlot->liCashItemSN);
	oPacket->Encode2(GetPosX());
	oPacket->Encode2(GetPosY() - 20);
	oPacket->Encode1(GetMoveAction());
	oPacket->Encode2(GetFh());
}

const GW_ItemSlotPet * Pet::GetItemSlot() const
{
	return m_pPetSlot;
}
