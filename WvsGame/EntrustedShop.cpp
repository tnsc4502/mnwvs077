#include "EntrustedShop.h"
#include "WvsGame.h"
#include "User.h"
#include "Field.h"
#include "FieldMan.h"
#include "LifePool.h"
#include "QWUser.h"
#include "QWUInventory.h"
#include "ExchangeElement.h"
#include "BackupItem.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\WvsCenter\EntrustedShopMan.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\FieldPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\GameSrvPacketFlags.hpp"

EntrustedShop::EntrustedShop()
{
}

EntrustedShop::~EntrustedShop()
{
}

bool EntrustedShop::IsEmployer(User *pUser) const
{
	return m_nEmployerID == pUser->GetUserID();
}

void EntrustedShop::OnPacket(User *pUser, int nType, InPacket *iPacket)
{
	switch (nType)
	{
		case EntrustedShopRequest::req_EShop_PutItem:
			PersonalShop::OnPutItem(pUser, iPacket);
			break;
		case EntrustedShopRequest::req_EShop_MoveItemToInventory:
			PersonalShop::OnMoveItemToInventory(pUser, iPacket);
			break;
		case EntrustedShopRequest::req_EShop_BuyItem:
			PersonalShop::OnBuyItem(pUser, iPacket);
			break;
		case EntrustedShopRequest::req_EShop_GoOut:
			OnGoOut(pUser, iPacket);
			break;
		case EntrustedShopRequest::req_EShop_ArrangeItem:
			OnArrangeItem(pUser, iPacket);
			break;
		case EntrustedShopRequest::req_EShop_WithdrawAll:
			OnWithdrawAll(pUser, iPacket);
			break;
		case EntrustedShopRequest::req_EShop_WithdrawMoney:
			OnWithdrawMoney(pUser, iPacket, true);
			break;
	}
}

void EntrustedShop::DoTransaction(User *pUser, int nSlot, Item *psItem, int nNumber)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	std::lock_guard<std::recursive_mutex> buyerLock(pUser->GetLock());

	std::vector<BackupItem> aBackup;
	std::vector<ExchangeElement> aExchange;
	std::vector<InventoryManipulator::ChangeLog> aLogAdd;


	//For buyer
	aExchange.push_back({});
	aExchange[0].m_nCount = nNumber * psItem->nSet;
	aExchange[0].m_pItem = psItem->pItem->MakeClone();

	int nMoneyCost = psItem->nPrice * psItem->nNumber * -1;
	if (m_liEShopMoney + (long long int)(nMoneyCost * -1) > (long long int)INT_MAX)
	{
		pUser->SendNoticeMessage("販售者楓幣已滿。");
		pUser->SendCharacterStat(true, 0);
		return;
	}

	if (QWUInventory::Exchange(m_apUser[nSlot], nMoneyCost, aExchange, &aLogAdd, nullptr, aBackup)) 
		pUser->SendNoticeMessage("購買失敗，請確認背包欄位是否足夠。");
	else
	{
		m_liEShopMoney += (nMoneyCost * -1);
		psItem->nNumber -= nNumber;
		if (psItem->pItem->nType != GW_ItemSlotBase::EQUIP)
			((GW_ItemSlotBundle*)psItem->pItem)->nNumber = psItem->nNumber;

		pUser->SendNoticeMessage("購買成功。");
		SoldItem soldItem;
		soldItem.nItemID = psItem->pItem->nItemID;
		soldItem.nNumber = nNumber;
		soldItem.nPrice = (nMoneyCost * -1);
		m_aSoldItem.push_back(soldItem);
	}
	BroadcastItemList();
}

void EntrustedShop::OnGoOut(User *pUser, InPacket *iPacket)
{
}

void EntrustedShop::OnArrangeItem(User *pUser, InPacket *iPacket)
{
	if (!IsEmployer(pUser))
		return;

	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	for (int i = 0; i < (int)m_aItem.size(); )
	{
		if (m_aItem[i].nNumber == 0)
		{
			m_aItem[i].pItem->Release();
			m_aItem.erase(m_aItem.begin() + i);
		}
		else
			++i;
	}
	OnWithdrawMoney(pUser, iPacket, false);

	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
	oPacket.Encode1(EntrustedShopResult::res_EShop_ArrangeItem);
	oPacket.Encode4((int)m_liEShopMoney);
	pUser->SendPacket(&oPacket);
}

void EntrustedShop::OnWithdrawAll(User *pUser, InPacket *iPacket)
{
	if (!IsEmployer(pUser))
		return;

	bool bFailed = false;
	if(!OnWithdrawMoney(pUser, nullptr, true))
		bFailed = true;
	else
	{
		std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
		std::vector<ExchangeElement> aExchange;
		std::vector<InventoryManipulator::ChangeLog> aChangeLog;
		std::vector<BackupItem> aBackup;

		for (auto& item : m_aItem)
			if (item.nNumber > 0)
			{
				aExchange.push_back({});
				aExchange.back().m_nCount = item.nNumber;
				aExchange.back().m_pItem = item.pItem->MakeClone();
			}

		if (QWUInventory::Exchange(
			pUser,
			(int)m_liEShopMoney,
			aExchange,
			&aChangeLog,
			nullptr,
			aBackup
		))
		{
			pUser->SendNoticeMessage("請確保背包欄位足夠。");
			pUser->SendCharacterStat(true, 0);
			bFailed = true;
		}

		OutPacket oPacket;
		oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
		oPacket.Encode1(EntrustedShopResult::res_EShop_WithdrawAll);
		oPacket.Encode1(bFailed ? 1 : 0);
		pUser->SendPacket(&oPacket);

		if(!bFailed)
			CloseRequest(pUser, 3, 0);
	}
}

bool EntrustedShop::OnWithdrawMoney(User *pUser, InPacket *iPacket, bool bSend)
{
	if (!IsEmployer(pUser))
		return false;
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	if (InventoryManipulator::RawIncMoney(pUser->GetCharacterData(), (int)m_liEShopMoney))
	{
		m_liEShopMoney = 0;
		if (bSend)
		{
			OutPacket oPacket;
			oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
			oPacket.Encode1(EntrustedShopResult::res_EShop_WithdrawMoney);
			pUser->SendPacket(&oPacket);
		}
		pUser->SendCharacterStat(true, QWUser::IncMoney(pUser, 0, true));
		return true;
	}
	return false;
}

int EntrustedShop::GetLeaveType() const
{
	return 1;
}

int EntrustedShop::IsAdmitted(User *pUser, InPacket *iPacket, bool bOnCreate)
{
	int nBaseAdmissionRet = MiniRoomBase::IsAdmitted(pUser, iPacket, bOnCreate);
	if (!nBaseAdmissionRet)
	{
		if (bOnCreate)
		{
			std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
			int nPOS = iPacket->Decode2();
			auto pItem = pUser->GetCharacterData()->GetItem(GW_ItemSlotBase::GW_ItemSlotType::CASH, nPOS);
			if (!pItem || 
				pItem->nItemID != iPacket->Decode4() || 
				pItem->nItemID / 10000 != 503 ||
				pUser->HasOpenedEntrustedShop())
			{
				return MiniRoomAdmissionResult::res_Admission_InvalidShopStatus; //InvalidEmployeeItem
			}

			SetEmployeeTemplateID(pItem->nItemID);
			SetEmployerID(pUser->GetUserID());
			SetEmployerName(pUser->GetName());
			SetEmployeeFieldID(pUser->GetField()->GetFieldID());
			m_nMiniRoomSpec = pItem->nItemID % 100;
			m_bOnCreate = bOnCreate;
			pUser->SetEntrustedShopOpened(true);
			++m_nCurUsers;
			m_anReserved[0] = pUser->GetUserID();

			return MiniRoomAdmissionResult::res_Admission_Success;
		}
	}
	return nBaseAdmissionRet;
}

GW_ItemSlotBase* EntrustedShop::MoveItemToShop(GW_ItemSlotBase* pItem, User* pUser, int nTI, int nPOS, int nNumber, int * nPOS2)
{
	int nDec = 0;
	*nPOS2 = nPOS;
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	QWUInventory::RawRemoveItem(pUser, nTI, nPOS, nNumber, &aChangeLog, nDec, nullptr);
	QWUInventory::SendInventoryOperation(pUser, true, aChangeLog);
	return pItem;
}

bool EntrustedShop::RestoreItemFromShop(User* pUser, PersonalShop::Item* psItem)
{
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	std::vector<BackupItem> aBackup;
	std::vector<ExchangeElement> aExchange;
	ExchangeElement elem;
	elem.m_pItem = psItem->pItem;
	elem.m_nCount = psItem->nSet * psItem->nNumber;
	aExchange.push_back(elem);

	int nNumber = psItem->pItem->nType == GW_ItemSlotBase::EQUIP ? 
		1 : ((GW_ItemSlotBundle*)psItem->pItem)->nNumber;

	if (QWUInventory::Exchange(pUser, 0, aExchange, &aChangeLog, nullptr, aBackup))
	{
		if (psItem->pItem->nType != GW_ItemSlotBase::EQUIP)
			((GW_ItemSlotBundle*)psItem->pItem)->nNumber = nNumber;
		return false;
	}
	return true;
}

void EntrustedShop::OnLeave(User *pUser, int nLeaveType)
{
	if (IsEmployer(pUser))
	{
		pUser->FlushCharacterData();
		if (nLeaveType == 3)
		{
			CloseShop();
			--m_nCurUsers;
		}
	}
}

void EntrustedShop::EncodeEnterResult(User *pUser, OutPacket *oPacket)
{
	if (IsEmployer(pUser))
		oPacket->Encode2(0); //Saved chat
	else
		oPacket->Encode2(0);
	oPacket->EncodeStr(m_sEmployerName);
	if (IsEmployer(pUser))
	{
		oPacket->Encode4(GameDateTime::GetTime() - m_tOpen);
		oPacket->Encode1(m_bOnCreate ? 1 : 0);
		EncodeSoldItemList(oPacket);
	}
	oPacket->EncodeStr(GetTitle());
	oPacket->Encode1(m_nSlotCount); //m_nSlotCount
	EncodeItemList(oPacket);
}

void EntrustedShop::EncodeItemList(OutPacket *oPacket)
{
	oPacket->Encode4((int)m_liEShopMoney);
	oPacket->Encode1((char)m_aItem.size());
	for (auto& item : m_aItem)
	{
		oPacket->Encode2(item.nNumber);
		oPacket->Encode2(item.nSet);
		oPacket->Encode4(item.nPrice);
		item.pItem->RawEncode(oPacket);
	}
}

void EntrustedShop::EncodeSoldItemList(OutPacket * oPacket)
{
	oPacket->Encode1((int)m_aSoldItem.size());
	for (auto& soldItem : m_aSoldItem)
	{
		oPacket->Encode4(soldItem.nItemID);
		oPacket->Encode2(soldItem.nNumber); 
		oPacket->Encode4(soldItem.nPrice);
		oPacket->EncodeStr(soldItem.m_sBuyer);
	}
	oPacket->Encode4((int)m_liEShopMoney);
}

void EntrustedShop::Encode(OutPacket *oPacket)
{
}

void EntrustedShop::Release()
{
}

void EntrustedShop::RegisterOpenTime()
{
	m_tOpen = GameDateTime::GetTime();
}

void EntrustedShop::CloseShop()
{
	auto pUser = User::FindUser(m_nEmployerID);
	if (pUser)
		pUser->SetEntrustedShopOpened(false);
	auto pField = FieldMan::GetInstance()->GetField(m_nFieldID);
	if (pField)
		pField->GetLifePool()->RemoveEmployee(m_nEmployerID);

	OutPacket oPacket;
	oPacket.Encode2(GameSrvSendPacketFlag::EntrustedShopRequest);
	oPacket.Encode1(EntrustedShopMan::EntrustedShopRequest::req_EShop_UnRegisterShop);
	oPacket.Encode4(m_nEmployerID);
	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
}

int EntrustedShop::GetEmployeeTemplateID() const
{
	return m_nEmployeeTemplateID;
}

void EntrustedShop::SetEmployeeTemplateID(int nTemplateID)
{
	m_nEmployeeTemplateID = nTemplateID;
}

int EntrustedShop::GetEmployerID() const
{
	return m_nEmployerID;
}

void EntrustedShop::SetEmployerID(int nEmployerID)
{
	m_nEmployerID = nEmployerID;
}

int EntrustedShop::GetEmployeeFieldID() const
{
	return m_nFieldID;
}

void EntrustedShop::SetEmployeeFieldID(int nFieldID)
{
	m_nFieldID = nFieldID;
}

void EntrustedShop::SetEmployerName(const std::string &sName)
{
	m_sEmployerName = sName;
}

const std::string& EntrustedShop::GetEmployerName() const
{
	return m_sEmployerName;
}
