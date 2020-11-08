#include "Pet.h"
#include "Field.h"
#include "User.h"
#include "MovePath.h"
#include "WvsPhysicalSpace2D.h"
#include "StaticFoothold.h"
#include "DropPool.h"
#include "PetTemplate.h"
#include "UserPacketTypes.hpp"
#include "QWUInventory.h"

#include "..\Database\GW_ItemSlotPet.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Random\Rand32.h"

#include <algorithm>

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
	m_pTemplate = PetTemplate::GetPetTemplate(m_pPetSlot->nItemID);
	if (!m_pTemplate)
		m_pOwner->SendNoticeMessage("Invalid PetTemplate.");

	return;

	this->m_ptPos.x = pUser->GetPosX();
	this->m_ptPos.y = pUser->GetPosY() - 20;
	OnEnterField(pUser->GetField());
}

void Pet::OnPacket(InPacket *iPacket, int nType)
{
	switch (nType)
	{
		case UserRecvPacketType::User_OnPetMove:
			OnMove(iPacket);
			break;
		case UserRecvPacketType::User_OnPetActionSpeak:
		case UserRecvPacketType::User_OnPetAction:
			break;
		case UserRecvPacketType::User_OnPetDropPickupRequest:
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

void Pet::OnLeaveField(int nRemoveReason)
{
	OutPacket oPacket;
	m_nRemoveReason = nRemoveReason;
	MakeLeaveFieldPacket(&oPacket);
	m_pField->SplitSendPacket(&oPacket, nullptr);
}

void Pet::OnMove(InPacket *iPacket)
{
	MovePath movePath;
	movePath.Decode(iPacket);
	ValidateMovePath(&movePath);
	OutPacket oPacket;
	oPacket.Encode2((short)UserSendPacketType::UserCommon_Pet_OnMove);
	oPacket.Encode4(m_pOwner->GetUserID());
	oPacket.Encode1(m_nIndex);
	movePath.Encode(&oPacket);

	m_pField->SplitSendPacket(&oPacket, m_pOwner);
}

void Pet::MakeEnterFieldPacket(OutPacket *oPacket)
{
	oPacket->Encode2((short)UserSendPacketType::UserCommon_Pet_OnMakeEnterFieldPacket);
	oPacket->Encode4(m_pOwner->GetUserID());
	oPacket->Encode1(m_nIndex);
	oPacket->Encode1(1);
	EncodeInitData(oPacket);
}

void Pet::MakeLeaveFieldPacket(OutPacket *oPacket)
{
	oPacket->Encode2((short)UserSendPacketType::UserCommon_Pet_OnMakeEnterFieldPacket);
	oPacket->Encode4(m_pOwner->GetUserID());
	oPacket->Encode1(m_nIndex);
	oPacket->Encode1(0);
	oPacket->Encode1(m_nRemoveReason);
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

void Pet::Update(unsigned int tCur, bool & bRemove)
{
	auto pPetSlot = (GW_ItemSlotPet*)m_pPetSlot;
	bool bUpdate = false;
	unsigned int tElapsed = tCur - m_tLastUpdated;
	m_tLastUpdated = tCur;
	if (m_tRemainHungriness < tElapsed)
	{
		int nRnd = (int)((unsigned int)Rand32::GetInstance()->Random() % (36 - 6 * m_pTemplate->nHungry)) + 60;
		m_tRemainHungriness = 1000 * nRnd;
		--pPetSlot->nRepleteness;
		bUpdate = true;
	}
	else
		m_tRemainHungriness -= tElapsed;

	if (pPetSlot->nRepleteness)
		bRemove = false;
	else
	{
		pPetSlot->nTameness = std::max(0, pPetSlot->nTameness - 1);
		pPetSlot->nRepleteness = 5;
		bUpdate = bRemove = true;
	}

	if (bUpdate)
		QWUInventory::UpdatePetItem(m_pOwner, pPetSlot->nPOS);
}

void Pet::OnEatFood(int niRepleteness)
{
	GW_ItemSlotPet* pPetSlot = m_pPetSlot;
	int nInc = niRepleteness;
	if (nInc + pPetSlot->nRepleteness > 100)
		nInc = 100 - pPetSlot->nRepleteness;

	pPetSlot->nRepleteness += nInc;
	m_tRemainHungriness = 1000 * (((int)Rand32::GetInstance()->Random() % 10) + m_nOvereat * m_nOvereat + 10);
	if ((((10 * nInc) / niRepleteness) <= (int)(Rand32::GetInstance()->Random() % 12)) ||
		(pPetSlot->nRepleteness / 10 <= (int)(Rand32::GetInstance()->Random() % 12)))
	{
		if (nInc == 0)
		{
			if ((int)(Rand32::GetInstance()->Random() % (int)std::max(1, 10 - m_nOvereat)))
				++m_nOvereat;
			else
			{
				pPetSlot->nTameness = std::max(0, pPetSlot->nTameness - 1);
				m_nOvereat = 0;
				QWUInventory::UpdatePetItem(m_pOwner, pPetSlot->nPOS);
			}
		}
	}
	else
		IncTameness(1);

	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserCommon_Pet_OnActionCommand);
	oPacket.Encode4(m_pOwner->GetUserID());
	oPacket.Encode1(GetIndex());
	oPacket.Encode1(1); //nCommand
	oPacket.Encode1(nInc > 0 ? 1 : 0);
	oPacket.Encode1(0);

	m_pOwner->SendPacket(&oPacket);
}

int Pet::GetLevel()
{
	const static int s_anEXP[] = {
		0x1, 0x3, 0x6, 0x0E, 0x1F, 0x3C, 0x6C, 0x0B5, 0x11F, 0x1B2, 0x278,
		0x37B, 0x4C8, 0x66A, 0x871, 0x0AE9, 0x0DE5, 0x1173, 0x15A6,
		0x1A91, 0x2047, 0x26DE, 0x2E6A, 0x3704, 0x40C2, 0x4BBF, 0x5813,
		0x65DA, 0x7530
	};

	GW_ItemSlotPet* pPetSlot = m_pPetSlot;
	int nTameness = pPetSlot->nTameness;
	int nIndex = 0;
	while (nTameness >= s_anEXP[nIndex])
		if (nIndex == sizeof(s_anEXP) / sizeof(int))
			return 30;
		else
			++nIndex;

	return nIndex + 1;
}

int Pet::IncTameness(int nInc)
{
	GW_ItemSlotPet* pPetSlot = m_pPetSlot;

	if (nInc + pPetSlot->nTameness >= 0)
	{
		if (nInc + pPetSlot->nTameness > 30000)
			nInc = 30000 - pPetSlot->nTameness;
	}
	else
		nInc = -pPetSlot->nTameness;

	if (nInc)
	{
		pPetSlot->nTameness += nInc;
		int nLevel = GetLevel();
		if (nLevel > pPetSlot->nLevel)
		{
			pPetSlot->nLevel = nLevel;
			OutPacket oPacket;
			oPacket.Encode2(UserSendPacketType::UserLocal_OnEffect);
			oPacket.Encode1(4);
			oPacket.Encode1(0);
			oPacket.Encode1(GetIndex());

			m_pOwner->SendPacket(&oPacket);
		}
	}

	return 0;
}

const GW_ItemSlotPet * Pet::GetItemSlot() const
{
	return m_pPetSlot;
}
