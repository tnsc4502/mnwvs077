#include "User.h"
#include "ClientSocket.h"
#include "WvsShop.h"
#include "ShopPacketTypes.hpp"
#include "ShopInfo.h"

#include "..\Database\GW_Memo.h"
#include "..\Database\GW_GiftList.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_CharacterLevel.h"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GW_ItemSlotPet.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_Avatar.hpp"
#include "..\Database\GW_CashItemInfo.h"
#include "..\Database\GW_FuncKeyMapped.h"
#include "..\Database\GW_WishList.h"

#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Memory\ZMemory.h"

#include "..\WvsCenter\CenterPacketTypes.hpp"

#include "..\WvsGame\UserPacketTypes.hpp"
#include "..\WvsGame\ItemInfo.h"

User::User(ClientSocket *_pSocket, InPacket *iPacket)
	: m_pSocket(_pSocket),
	m_pCharacterData(AllocObj(GA_Character)),
	m_pWishList(AllocObj(GW_WishList))
{
	_pSocket->SetUser(this);
	m_pFuncKeyMapped.reset(MakeUnique<GW_FuncKeyMapped>(m_pCharacterData->nCharacterID));
	m_pCharacterData->nAccountID = iPacket->Decode4();
	m_pCharacterData->DecodeCharacterData(iPacket, true);
	m_pFuncKeyMapped->Decode(iPacket);
	if (!iPacket->Decode1())
		m_nChannelID = 0;
	else
		m_nChannelID = iPacket->Decode4();
	auto bindT = std::bind(&User::Update, this);
	m_pUpdateTimer.reset(AsyncScheduler::CreateTask(bindT, 2000, true));
	m_pUpdateTimer->Start();

	OnRequestCenterUpdateCash();
	OnQueryCashRequest();
	OnRequestCenterLoadLocker();

	//ValidateState();

	OutPacket oPacket;
	oPacket.Encode2(ShopSendPacketType::Client_SetCashShop);
	m_pCharacterData->EncodeCharacterData(&oPacket, false);
	oPacket.EncodeStr("Maple");
	oPacket.Encode4(0); //0082C69E NotLimited
	ShopInfo::GetInstance()->EncodeModifiedCommodity(&oPacket);
	oPacket.Encode1(0);
	oPacket.EncodeBuffer(nullptr, 1080); //004599FF BestItems
	oPacket.Encode2(0); //45A1B7
	oPacket.Encode2(0); //45A1FE
	oPacket.Encode2(0); //
	SendPacket(&oPacket);
	//m_pSecondaryStat->DecodeInternal(this, iPacket);

	RequestLoadGiftList();
	RequestLoadWishList();
}

User::~User()
{
	OutPacket oSetWishList;
	oSetWishList.Encode2(CenterRequestPacketType::CashItemRequest);
	oSetWishList.Encode4(m_pSocket->GetSocketID());
	oSetWishList.Encode4(GetUserID());
	oSetWishList.Encode2(CenterCashItemRequestType::eSetWishItemRequest);
	m_pWishList->Encode(&oSetWishList);
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oSetWishList);

	OutPacket oPacket;
	oPacket.Encode2((short)CenterRequestPacketType::RequestMigrateOut);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(GetUserID());
	oPacket.Encode4(-1);
	m_pCharacterData->EncodeCharacterData(&oPacket, true);
	m_pFuncKeyMapped->Encode(&oPacket, true);
	oPacket.Encode1(m_bTransferChannel ? 
		CenterMigrationType::eMigrateOut_TransferChannelFromShop : 
		CenterMigrationType::eMigrateOut_ClientDisconnected
	);
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);

	m_pUpdateTimer->Abort();
}

User * User::FindUser(int nUserID)
{
	return WvsBase::GetInstance<WvsShop>()->FindUser(nUserID);
}

ZUniquePtr<GA_Character>& User::GetCharacterData()
{
	return m_pCharacterData;
}

int User::GetUserID() const
{
	return m_pCharacterData->nCharacterID;
}

int User::GetAccountID() const
{
	return m_pCharacterData->nAccountID;
}

void User::SendPacket(OutPacket *oPacket)
{
	m_pSocket->SendPacket(oPacket);
}

void User::Update()
{
}

void User::OnMigrateOutCashShop()
{
	OutPacket oPacket;
	oPacket.Encode2((short)CenterRequestPacketType::RequestTransferChannel);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(m_pCharacterData->nCharacterID);
	oPacket.Encode1(m_nChannelID);
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
	m_bTransferChannel = true;
}

void User::ValidateState()
{
	OutPacket oPacket;
	oPacket.Encode2((short)ShopSendPacketType::Client_ValidateState);
	oPacket.Encode1(1);
	oPacket.Encode4(0);
	SendPacket(&oPacket);
}

void User::OnQueryCashRequest()
{
	OutPacket oPacket;
	oPacket.Encode2(ShopSendPacketType::User_QueryCashResult);
	oPacket.Encode4(m_nNexonCash);
	oPacket.Encode4(m_nMaplePoint);
	oPacket.Encode4(0);
	oPacket.Encode4(0);
	SendPacket(&oPacket);
}

void User::OnPacket(InPacket *iPacket)
{
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
		case UserRecvPacketType::User_OnUserTransferFieldRequest:
			OnMigrateOutCashShop();
			break;
		case ShopRecvPacketType::User_OnCashItemRequest:
			OnUserCashItemRequest(iPacket);
			break;
		case ShopRecvPacketType::User_OnQueryCashReques:
			OnQueryCashRequest();
			break;
		case UserRecvPacketType::User_OnMemoRequest:
			OnMemoRequest(iPacket);
			break;
	}
	ValidateState();
}

void User::OnUserCashItemRequest(InPacket * iPacket)
{
	int nType = iPacket->Decode1();
	switch (nType)
	{
		case CashItemRequest::Recv_OnCashItemReqBuy:
			OnRequestBuyCashItem(iPacket);
			break;
		case CashItemRequest::Recv_OnCashItemReqMoveItemToSlot:
			OnRequestMoveItemToSlot(iPacket);
			break;
		case CashItemRequest::Recv_OnCashItemReqMoveItemToLocker:
			OnRequestMoveItemToLocker(iPacket);
			break;
		case CashItemRequest::Recv_OnCashItemReqGift:
			OnRequestCashItemGift(iPacket);
			break;
		case CashItemRequest::Recv_OnCashItemReqBuyCashPackage:
			OnRequestBuyCashPackage(iPacket);
			break;
		case CashItemRequest::Recv_OnCashItemReqGiftCashPackage:
			OnRequestGiftCashPackage(iPacket);
			break;
		case CashItemRequest::Recv_OnCashItemReqSetWish:
			OnRequestSetWishList(iPacket);
			break;
	}
}

void User::OnCenterCashItemResult(int nType, InPacket * iPacket)
{
	bool bValidate = true;
	switch (nType)
	{
		case CenterCashItemRequestType::eLoadCashItemLockerRequest:
			OnCenterResLoadLockerDone(iPacket);
			break;
		case CenterCashItemRequestType::eBuyCashItemRequest:
			OnCenterResBuyDone(iPacket);
			break;
		case CenterCashItemRequestType::eGetMaplePointRequest:
			OnCenterUpdateCashDone(iPacket);
			break;
		case CenterCashItemRequestType::eMoveCashItemLtoSRequest:
			OnCenterMoveItemToSlotDone(iPacket);
			break;
		case CenterCashItemRequestType::eMoveCashItemStoLRequest:
			OnCenterMoveItemToLockerDone(iPacket);
			break;
		case CenterCashItemRequestType::eGiftCashItemRequest:
		case CenterCashItemRequestType::eGiftCashPackageRequest:
			OnCenterGiftCashItemDone(iPacket);
			break;
		case CenterCashItemRequestType::eLoadGiftListRequest:
			OnCenterGiftListResult(iPacket);
			break;
		case CenterCashItemRequestType::eBuyCashPackageRequest:
			OnCenterBuyCashPackageDone(iPacket);
			break;
		case CenterCashItemRequestType::eLoadWishItemRequest:
			OnRequestSetWishList(iPacket, true);
			break;
		default:
			bValidate = false;
			break;
	}
	OnQueryCashRequest();
	if(bValidate)
		ValidateState();
}

void User::OnCenterResLoadLockerDone(InPacket * iPacket)
{
	short nFailedReason = iPacket->Decode1();

	OutPacket oPacket;
	oPacket.Encode2((short)ShopSendPacketType::User_CashItemResult);
	oPacket.Encode1(CashItemRequest::Send_OnCashItemResLoadLockerDone);
	oPacket.EncodeBuffer(
		iPacket->GetPacket() + iPacket->GetReadCount(),
		iPacket->GetPacketSize() - iPacket->GetReadCount());
	SendPacket(&oPacket);
}

void User::OnCenterResBuyDone(InPacket * iPacket)
{
	short nFailedReason = iPacket->Decode1();
	int nItemCount = iPacket->Decode1();

	m_nNexonCash = iPacket->Decode4();
	m_nMaplePoint = iPacket->Decode4();

	OutPacket oPacket;
	oPacket.Encode2(ShopSendPacketType::User_CashItemResult);
	oPacket.Encode1(CashItemRequest::Send_OnCashItemResBuyDone);

	//Transfer GW_CashItemInfo
	oPacket.EncodeBuffer(
		iPacket->GetPacket() + iPacket->GetReadCount(),
		iPacket->GetPacketSize() - iPacket->GetReadCount()
	);

	oPacket.EncodeBuffer(nullptr, 25);
	SendPacket(&oPacket);
}

void User::OnCenterGiftCashItemDone(InPacket * iPacket)
{
	short nFailedReason = iPacket->Decode1();
	int nItemCount = iPacket->Decode1();

	m_nNexonCash = iPacket->Decode4();
	m_nMaplePoint = iPacket->Decode4();

	int nGiftPacketSize = (iPacket->GetPacketSize() - iPacket->GetReadCount()) / nItemCount;
	for (int i = 0; i < nItemCount; ++i)
	{
		OutPacket oPacket;
		oPacket.Encode2(ShopSendPacketType::User_CashItemResult);
		oPacket.Encode1(CashItemRequest::Send_OnCashItemResGiftDone);
		oPacket.EncodeBuffer(iPacket->GetPacket() + iPacket->GetReadCount() + nGiftPacketSize * i, nGiftPacketSize);
		SendPacket(&oPacket);
	}
}

void User::OnCenterBuyCashPackageDone(InPacket * iPacket)
{
	short nFailedReason = iPacket->Decode1();
	int nItemCount = iPacket->Decode1();

	m_nNexonCash = iPacket->Decode4();
	m_nMaplePoint = iPacket->Decode4();

	OutPacket oPacket;
	oPacket.Encode2(ShopSendPacketType::User_CashItemResult);
	oPacket.Encode1(CashItemRequest::Send_OnCashItemResBuyPackageDone);
	oPacket.Encode1(nItemCount);
	oPacket.EncodeBuffer(iPacket->GetPacket() + iPacket->GetReadCount(), iPacket->GetPacketSize() - iPacket->GetReadCount());
	oPacket.Encode2(0); //nMaplePoint
	SendPacket(&oPacket);
}

void User::OnCenterUpdateCashDone(InPacket * iPacket)
{
	short nFailedReason = iPacket->Decode1();

	m_nNexonCash = iPacket->Decode4();
	m_nMaplePoint = iPacket->Decode4();
	iPacket->Decode4();
	iPacket->Decode4();
	OnQueryCashRequest();
}

void User::OnCenterMoveItemToSlotDone(InPacket * iPacket)
{
	short nFailedReason = iPacket->Decode1();

	OutPacket oPacket;
	oPacket.Encode2((short)ShopSendPacketType::User_CashItemResult);
	oPacket.Encode1(CashItemRequest::Send_OnCashItemResMoveLtoSDone);
	long long int liItemSN = iPacket->Decode8();
	oPacket.EncodeBuffer(
		iPacket->GetPacket() + iPacket->GetReadCount(),
		iPacket->GetPacketSize() - iPacket->GetReadCount()
	);
	int nPOS = iPacket->Decode2();
	int nInstanceType = iPacket->Decode1();
	iPacket->Offset(-1);
	ZSharedPtr<GW_ItemSlotBase> pItem = GW_ItemSlotBase::CreateItem(nInstanceType);
	if (pItem)
	{
		pItem->Decode(iPacket, false);
		pItem->liItemSN = liItemSN;
		pItem->nCharacterID = m_pCharacterData->nCharacterID;
		pItem->nPOS = nPOS;
		pItem->nType = (GW_ItemSlotBase::GW_ItemSlotType)((pItem->nItemID / 1000000));
		m_pCharacterData->mItemSlot[pItem->nType][nPOS] = pItem;
	}

	SendPacket(&oPacket);
}

void User::OnCenterMoveItemToLockerDone(InPacket * iPacket)
{
	short nFailedReason = iPacket->Decode1();

	auto liCashItemSN = iPacket->Decode8();
	int nType = iPacket->Decode1();
	short nPOS = m_pCharacterData->FindCashItemSlotPosition(nType, liCashItemSN);
	if (nPOS)
		m_pCharacterData->mItemSlot[nType].erase(nPOS);

	OutPacket oPacket;
	oPacket.Encode2((short)ShopSendPacketType::User_CashItemResult);
	oPacket.Encode1(CashItemRequest::Send_OnCashItemResMoveStoLDone);
	oPacket.EncodeBuffer(
		iPacket->GetPacket() + iPacket->GetReadCount(),
		iPacket->GetPacketSize() - iPacket->GetReadCount()
	);
	SendPacket(&oPacket);
}

void User::OnCenterGiftListResult(InPacket * iPacket)
{
	OutPacket oPacket;
	oPacket.Encode2(ShopSendPacketType::User_CashItemResult);
	oPacket.Encode1(CashItemRequest::Send_OnCashItemResLoadGiftDone);
	oPacket.EncodeBuffer(
		iPacket->GetPacket() + iPacket->GetReadCount(),
		iPacket->GetPacketSize() - iPacket->GetReadCount()
	);

	m_mGiftList.clear();
	int nCount = iPacket->Decode2();
	for (int i = 0; i < nCount; ++i)
	{
		auto pGList = AllocObj(GW_GiftList);
		pGList->Decode(iPacket);
		m_mGiftList.insert({ i, pGList });
	}
	SendPacket(&oPacket);
}

void User::OnRequestCenterLoadLocker()
{
	OutPacket oPacket;
	oPacket.Encode2((short)CenterRequestPacketType::CashItemRequest);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(this->m_pCharacterData->nCharacterID);
	oPacket.Encode2((short)CenterCashItemRequestType::eLoadCashItemLockerRequest);
	oPacket.Encode4(this->m_pCharacterData->nAccountID);
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
}

void User::OnRequestCenterUpdateCash()
{
	OutPacket oPacket;
	oPacket.Encode2((short)CenterRequestPacketType::CashItemRequest);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(this->m_pCharacterData->nCharacterID);
	oPacket.Encode2((short)CenterCashItemRequestType::eGetMaplePointRequest);
	oPacket.Encode4(this->m_pCharacterData->nAccountID);
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
}

void User::RequestLoadGiftList()
{
	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::CashItemRequest);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(GetUserID());
	oPacket.Encode2(CenterCashItemRequestType::eLoadGiftListRequest);
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
}

void User::RequestLoadWishList()
{
	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::CashItemRequest);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(GetUserID());
	oPacket.Encode2(CenterCashItemRequestType::eLoadWishItemRequest);
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
}

void User::OnMemoRequest(InPacket *iPacket)
{
	int nType = iPacket->Decode1();
	if (nType == 0)
	{
		std::string sReceiver = iPacket->DecodeStr();
		std::string sContent = iPacket->DecodeStr();
		int nFlag = iPacket->Decode1();
		int nMemoIdx = iPacket->Decode4();
		long long int liItemSN = iPacket->Decode8();

		auto findIter = m_mGiftList.find(nMemoIdx);
		if(findIter == m_mGiftList.end())
			return;


		OutPacket oPacket;
		oPacket.Encode2(CenterRequestPacketType::MemoRequest);
		oPacket.Encode4(GetUserID());
		oPacket.Encode1(GW_Memo::MemoRequestType::eMemoReq_Send);
		oPacket.EncodeStr(sReceiver);
		oPacket.EncodeStr(m_pCharacterData->strName);
		oPacket.EncodeStr(sContent);
		oPacket.Encode8(GameDateTime::GetCurrentDate());
		oPacket.Encode1(nFlag);
		oPacket.Encode2(0);

		WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
		m_mGiftList.erase(nMemoIdx);
	}
}

bool User::EncodeBuyCashItem(OutPacket* oPacket, const std::vector<const CSCommodity *>& apCommodity, int nRequestType, int nChargeType, int nPrice, bool bGift, const std::string & sReceiver, const std::string & sMemo)
{
	oPacket->Encode2((short)CenterRequestPacketType::CashItemRequest);
	oPacket->Encode4(m_pSocket->GetSocketID());
	oPacket->Encode4(this->m_pCharacterData->nCharacterID);
	oPacket->Encode2((short)nRequestType);
	oPacket->Encode4(this->m_pCharacterData->nAccountID);
	oPacket->EncodeStr(m_pCharacterData->strName);
	if (bGift)
	{
		oPacket->EncodeStr(sReceiver);
		oPacket->EncodeStr(sMemo);
	}
	oPacket->Encode1(nChargeType);
	oPacket->Encode4(nPrice);
	oPacket->Encode1((char)apCommodity.size());

	for (auto pCommodity : apCommodity)
	{
		oPacket->Encode1(pCommodity->nItemID / 1000000);

		ZSharedPtr<GW_ItemSlotBase> pItem = ItemInfo::GetInstance()->GetItemSlot(
			pCommodity->nItemID,
			ItemInfo::ItemVariationOption::ITEMVARIATION_NONE);

		if (!pItem)
			return false;

		oPacket->Encode1(pItem->bIsPet);
		pItem->Encode(oPacket, false);
		ZUniquePtr<GW_CashItemInfo> pCashItemInfo = ShopInfo::GetInstance()->GetCashItemInfo(pCommodity);
		pCashItemInfo->Encode(oPacket);
	}

	return true;
}

void User::OnRequestBuyCashItem(InPacket * iPacket)
{
	int nChargeType = iPacket->Decode1();
	auto pCommodity = ShopInfo::GetInstance()->GetCSCommodity(iPacket->Decode4());
	if (pCommodity)
	{
		OutPacket oPacket;
		if (EncodeBuyCashItem(&oPacket, { pCommodity }, CenterCashItemRequestType::eBuyCashItemRequest, nChargeType, pCommodity->nPrice))
			WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
	}
}

void User::OnRequestMoveItemToSlot(InPacket * iPacket)
{
	OutPacket oPacket;
	oPacket.Encode2((short)CenterRequestPacketType::CashItemRequest);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(this->m_pCharacterData->nCharacterID);
	oPacket.Encode2((short)CenterCashItemRequestType::eMoveCashItemLtoSRequest);
	oPacket.Encode4(this->m_pCharacterData->nAccountID);
	oPacket.Encode8(iPacket->Decode8()); //liCashItemSN
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
}

void User::OnRequestMoveItemToLocker(InPacket * iPacket)
{
	OutPacket oPacket;
	oPacket.Encode2((short)CenterRequestPacketType::CashItemRequest);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(this->m_pCharacterData->nCharacterID);
	oPacket.Encode2((short)CenterCashItemRequestType::eMoveCashItemStoLRequest);
	oPacket.Encode4(this->m_pCharacterData->nAccountID);
	oPacket.Encode8(iPacket->Decode8()); //liCashItemSN
	oPacket.Encode1(iPacket->Decode1()); //nType
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
}

void User::OnRequestCashItemGift(InPacket * iPacket)
{
	std::string s2ndPass = iPacket->DecodeStr();
	int nSN = iPacket->Decode4();
	std::string sReceiver = iPacket->DecodeStr();
	std::string sMemo = iPacket->DecodeStr();

	auto pCommodity = ShopInfo::GetInstance()->GetCSCommodity(nSN);
	if (pCommodity)
	{
		OutPacket oPacket;
		if (EncodeBuyCashItem(&oPacket, { pCommodity }, CenterCashItemRequestType::eGiftCashItemRequest, 1, pCommodity->nPrice, true, sReceiver, sMemo))
			WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
	}
}

void User::OnRequestBuyCashPackage(InPacket * iPacket)
{
	int nChargeType = iPacket->Decode1();
	int nSN = iPacket->Decode4();
	auto pCommodity = ShopInfo::GetInstance()->GetCSCommodity(nSN);
	if (!pCommodity || pCommodity->nItemID / 1000000 != 9)
		return;

	auto& aCommodity = ShopInfo::GetInstance()->GetCashPackage(pCommodity->nItemID);
	if (aCommodity.size())
	{
		OutPacket oPacket;
		if (EncodeBuyCashItem(&oPacket, { aCommodity }, CenterCashItemRequestType::eBuyCashPackageRequest, nChargeType, pCommodity->nPrice))
			WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
	}
}

void User::OnRequestGiftCashPackage(InPacket * iPacket)
{
	std::string s2ndPass = iPacket->DecodeStr();
	int nSN = iPacket->Decode4();
	std::string sReceiver = iPacket->DecodeStr();
	std::string sMemo = iPacket->DecodeStr();
	auto pCommodity = ShopInfo::GetInstance()->GetCSCommodity(nSN);
	if (!pCommodity || pCommodity->nItemID / 1000000 != 9)
		return;

	auto& aCommodity = ShopInfo::GetInstance()->GetCashPackage(pCommodity->nItemID);
	if (aCommodity.size())
	{
		OutPacket oPacket;
		if (EncodeBuyCashItem(&oPacket, { aCommodity }, CenterCashItemRequestType::eGiftCashPackageRequest, 1, pCommodity->nPrice, true, sReceiver, sMemo))
			WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
	}
}

void User::OnRequestSetWishList(InPacket * iPacket, bool bLoad)
{
	if (!m_pWishList)
		return;

	m_pWishList->Decode(iPacket);
	OutPacket oPacket;
	oPacket.Encode2(ShopSendPacketType::User_CashItemResult);
	oPacket.Encode1(bLoad ? CashItemRequest::Send_OnCashItemResLoadWishDone : CashItemRequest::Send_OnCashItemResSetWishDone);
	for (int i = 0; i < GW_WishList::MAX_WISHLIST_COUNT; ++i)
		oPacket.Encode4(m_pWishList->aWishList[i]);

	SendPacket(&oPacket);
}