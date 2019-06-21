#include "Drop.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\WvsLib\Net\PacketFlags\DropPacketFlags.hpp"
#include "Reward.h"
#include "User.h"
#include "Pet.h"
#include "QWUQuestRecord.h"
#include "QWUser.h"

Drop::Drop()
{
}


Drop::~Drop()
{
}

void Drop::MakeEnterFieldPacket(OutPacket * oPacket)
{
	MakeEnterFieldPacket(oPacket, 2, 0);
}

void Drop::MakeEnterFieldPacket(OutPacket * oPacket, int nEnterType, int tDelay)
{
	oPacket->Encode2((short)DropSendPacketFlag::Drop_OnMakeEnterFieldPacket); //Drop Pool :: Enter Field
	oPacket->Encode1(nEnterType);
	oPacket->Encode4(m_dwDropID);
	oPacket->Encode1(m_bIsMoney);
	oPacket->Encode4(m_pItem != nullptr ? m_pItem->nItemID : m_nMoney);
	oPacket->Encode4(m_nOwnType ? m_dwOwnPartyID : m_dwOwnerID);
	oPacket->Encode1(m_nOwnType);
	oPacket->Encode2(m_pt2.x);
	oPacket->Encode2(m_pt2.y);
	oPacket->Encode4(m_dwSourceID);
	if (!nEnterType || nEnterType == 1 || nEnterType == 3)
	{
		oPacket->Encode2(m_pt1.x);
		oPacket->Encode2(m_pt1.y);
		oPacket->Encode2(tDelay);
	}
	if(!m_bIsMoney)
		oPacket->Encode8(m_pItem->liExpireDate);
	oPacket->Encode1(m_bByPet);
}

void Drop::MakeLeaveFieldPacket(OutPacket *oPacket)
{
	MakeLeaveFieldPacket(oPacket, 1, 0, nullptr);
}

void Drop::MakeLeaveFieldPacket(OutPacket *oPacket, int nLeaveType, int nOption, Pet *pPet)
{
	oPacket->Encode2((short)DropSendPacketFlag::Drop_OnMakeLeaveFieldPacket);
	oPacket->Encode1((char)nLeaveType);
	oPacket->Encode4(m_dwDropID);
	if (nLeaveType >= 2 && nLeaveType != 4) 
	{
		oPacket->Encode4(nOption);
		if (nLeaveType == 5 && pPet)
			oPacket->Encode1(pPet->GetIndex());
	}
	else if (nLeaveType == 4)
		oPacket->Encode2(nOption);
}

bool Drop::IsShowTo(FieldObj *pUser_)
{
	auto pUser = (User*)pUser_;
	if (!pUser || QWUser::GetHP(pUser) == 0)
		return false;
	if (!m_usQRKey)
		return true;

	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	if (m_usQRKey && QWUQuestRecord::GetState(pUser, m_usQRKey) == 1)
		return true;
		
	return false;
}

void Drop::Init(unsigned int dwDropID, Reward * reward, unsigned int dwOwnerID, unsigned int dwOwnPartyID, int nOwnType, unsigned int dwSourceID, int x1, int y1, int x2, int y2, int bByPet)
{
	SetFieldObjectID(dwDropID);
	this->m_dwDropID = dwDropID;
	this->m_dwOwnerID = dwOwnerID;
	this->m_dwOwnPartyID = dwOwnPartyID;
	this->m_nOwnType = nOwnType;
	this->m_dwSourceID = dwSourceID;
	this->m_bByPet = bByPet != 0;
	this->m_pt1.x = x1;
	this->m_pt1.y = y1;
	this->m_pt2.x = x2;
	this->m_pt2.y = y2;
	SetPosX(x2);
	SetPosY(y2);
	this->m_bIsMoney = reward->GetMoney() != 0;
	this->m_nMoney = reward->GetMoney();
	this->m_usQRKey = 0;
	this->m_nShowMax = 0;
	this->m_nPeriod = reward->GetPeriod();
	auto pInfo = reward->GetRewardInfo();
	if (pInfo)
	{
		m_usQRKey = pInfo->m_usQRKey;
		m_nShowMax = pInfo->m_nMaxCount;
	}
	m_pItem = reward->GetItem();
}

ZSharedPtr<GW_ItemSlotBase> Drop::GetItem()
{
	return m_pItem;
}

int Drop::GetDropInfo() const
{
	if (m_nMoney > 0)
		return m_nMoney;
	if (m_pItem)
		return m_pItem->nItemID;
	return 0;
}

int Drop::GetMoney() const
{
	return m_nMoney;
}
