#include "CashItemDBAccessor.h"
#include "CharacterDBAccessor.h"
#include "MemoDBAccessor.h"
#include "WvsUnified.h"
#include "GW_Account.h"
#include "GW_CashItemInfo.h"
#include "GW_ItemSlotEquip.h"
#include "GW_ItemSlotBundle.h"
#include "GW_ItemSlotPet.h"
#include "GA_Character.hpp"
#include "GW_GiftList.h"

#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\SocketBase.h"
#include "..\WvsCenter\CenterPacketTypes.hpp"
#include "..\WvsLib\Memory\ZMemory.h"

void CashItemDBAccessor::PostBuyCashItemRequest(SocketBase * pSrv, int uClientSocketSN, int nCharacterID, void * iPacket_, bool bGift)
{
	InPacket *iPacket = (InPacket*)iPacket_;
	OutPacket oPacket;
	oPacket.Encode2(CenterResultPacketType::CashItemResult);
	oPacket.Encode4(uClientSocketSN);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode2(bGift ? CenterCashItemRequestType::eGiftCashItemRequest : CenterCashItemRequestType::eBuyCashItemRequest);

	int nAccountID = iPacket->Decode4(), nReceiverID = 0, nReceiverAccountID = 0;
	GW_Account account;
	account.Load(nAccountID);

	std::string sBuyCharacterID = iPacket->DecodeStr(), sReceiver, sMemo;
	if (bGift)
	{
		sReceiver = iPacket->DecodeStr();
		nReceiverID = CharacterDBAccessor::QueryCharacterIDByName(sReceiver);
		nReceiverAccountID = nReceiverID < 0 ? -1 : CharacterDBAccessor::QueryCharacterAccountID(nReceiverID);
		if (nReceiverID == -1 || nReceiverAccountID == -1)
		{
			oPacket.Encode1(0);
			goto DELIVER_PACKET;
		}
		sMemo = iPacket->DecodeStr();
	}
	int nChargeType = iPacket->Decode1();
	int nType = iPacket->Decode1();
	bool bIsPet = iPacket->Decode1() == 1;
	int nPrice = iPacket->Decode4();

	if (account.QueryCash(nChargeType) >= nPrice)
	{
		account.UpdateCash(nChargeType, -nPrice);
		oPacket.Encode1(0); //Success
		ZSharedPtr<GW_ItemSlotBase> pItem;
		if (nType == GW_ItemSlotBase::GW_ItemSlotType::EQUIP)
			pItem = MakeShared<GW_ItemSlotEquip>();
		else if (bIsPet)
			pItem = MakeShared<GW_ItemSlotPet>();
		else
			pItem = MakeShared<GW_ItemSlotBundle>();

		pItem->nType = (nType == GW_ItemSlotBase::GW_ItemSlotType::EQUIP ?
			GW_ItemSlotBase::GW_ItemSlotType::EQUIP
			: GW_ItemSlotBase::GW_ItemSlotType::CASH);
		pItem->bIsCash = true;
		pItem->bIsPet = bIsPet;
		pItem->DecodeInventoryPosition(iPacket);
		pItem->Decode(iPacket, false);
		pItem->nCharacterID = bGift ? nReceiverID : nCharacterID;
		pItem->liCashItemSN = GW_ItemSlotBase::IncItemSN(GW_ItemSlotBase::GW_ItemSlotType::CASH);
		pItem->nPOS = GW_ItemSlotBase::LOCK_POS;
		pItem->liItemSN = -1;

		GW_CashItemInfo cashItemInfo;
		cashItemInfo.sBuyCharacterID = sBuyCharacterID;
		cashItemInfo.Decode(iPacket);
		cashItemInfo.nAccountID = bGift ? nReceiverAccountID : nAccountID;
		cashItemInfo.cashItemOption.liCashItemSN = pItem->liCashItemSN;
		cashItemInfo.bLocked = true;
		cashItemInfo.nGWItemSlotInstanceType = pItem->nInstanceType;
		cashItemInfo.Save(true);

		//09/12/2019 added. fix for bundled items.
		if (nType == GW_ItemSlotBase::GW_ItemSlotType::CASH && !bIsPet)
			((GW_ItemSlotBundle*)pItem)->nNumber = cashItemInfo.nNumber;

		//A pet will only have ExpireDate in GW_ItemSlotPet(which stands for it's life time)
		if (!pItem->bIsPet)
			pItem->liExpireDate = cashItemInfo.liDateExpire;
		pItem->Save(pItem->nCharacterID);

		oPacket.Encode4(account.QueryCash(1));
		oPacket.Encode4(account.QueryCash(2));
		if (bGift)
		{
			oPacket.EncodeStr(sReceiver);
			oPacket.Encode4(pItem->nItemID);
			oPacket.Encode2(
				nType == GW_ItemSlotBase::GW_ItemSlotType::CASH && !bIsPet ?
				((GW_ItemSlotBundle*)pItem)->nNumber : 1
			);
			oPacket.Encode2(0);
			oPacket.Encode4(nPrice);
		}
		else
			cashItemInfo.Encode(&oPacket);

		if (sMemo != "")
		{
			GW_GiftList GList;
			GList.nCharacterID = nReceiverID;
			GList.liItemSN = pItem->liCashItemSN;
			GList.nItemID = pItem->nItemID;
			GList.sBuyCharacterName = sBuyCharacterID;
			GList.sText = sMemo;
			GList.nState = 1;
			GList.Save();
		}
	}
	else
		oPacket.Encode1(1);

DELIVER_PACKET:
	pSrv->SendPacket(&oPacket);
}


void CashItemDBAccessor::PostLoadLockerRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket_)
{
	InPacket *iPacket = (InPacket*)iPacket_;
	int nAccountID = iPacket->Decode4();
	auto aRes = GW_CashItemInfo::LoadAll(nAccountID);
	std::vector<GW_ItemSlotPet> aPet;
	OutPacket oPacket;
	oPacket.Encode2((short)CenterResultPacketType::CashItemResult);
	oPacket.Encode4(uClientSocketSN);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode2(CenterCashItemRequestType::eLoadCashItemLockerRequest);
	oPacket.Encode1(0); //FailedReason
	oPacket.Encode2((short)aRes.size());
	decltype(aRes) aResWithoutPet;
	for (auto& info : aRes)
	{
		if (info.nGWItemSlotInstanceType != GW_ItemSlotBase::GW_ItemSlotInstanceType::GW_ItemSlotPet_Type)
			aResWithoutPet.push_back(info);
		info.Encode(&oPacket);
		if (info.nGWItemSlotInstanceType == GW_ItemSlotBase::GW_ItemSlotInstanceType::GW_ItemSlotPet_Type)
		{
			aPet.push_back({});
			aPet[(int)aPet.size() - 1].Load(info.cashItemOption.liCashItemSN);
		}
	}
	//oPacket.Encode2(aResWithoutPet.size());
	//for (auto& info : aResWithoutPet)
	//	info.Encode(&oPacket);
	oPacket.Encode4(0);
	//for (auto& pet : aPet)
	//	pet.Encode(&oPacket, false);

	/*oPacket.Encode2(0);
	oPacket.Encode2(0);
	oPacket.Encode2(0);
	oPacket.Encode2(0);*/

	pSrv->SendPacket(&oPacket);
}

void CashItemDBAccessor::PostUpdateCashRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket_)
{
	InPacket *iPacket = (InPacket*)iPacket_;
	int nAccountID = iPacket->Decode4();
	GW_Account account;
	account.Load(nAccountID);

	OutPacket oPacket;
	oPacket.Encode2((short)CenterResultPacketType::CashItemResult);
	oPacket.Encode4(uClientSocketSN);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode2(CenterCashItemRequestType::eGetMaplePointRequest);

	oPacket.Encode1(0); //FailedReason
	oPacket.Encode4(account.nNexonCash);
	oPacket.Encode4(account.nMaplePoint);
	oPacket.Encode4(0);
	oPacket.Encode4(0);

	pSrv->SendPacket(&oPacket);
}

void CashItemDBAccessor::PostMoveSlotToLockerRequest(SocketBase * pSrv, int uClientSocketSN, int nCharacterID, void * iPacket_)
{
	InPacket *iPacket = (InPacket*)iPacket_;
	int nAccountID = iPacket->Decode4();
	long long int liCashItemSN = iPacket->Decode8();
	int nType = iPacket->Decode1();
	GW_CashItemInfo cashItemInfo;
	cashItemInfo.Load(liCashItemSN);
	cashItemInfo.bLocked = true;
	cashItemInfo.Save();

	ZSharedPtr<GW_ItemSlotBase> pItem;
	pItem.reset(GW_ItemSlotBase::CreateItem(cashItemInfo.nGWItemSlotInstanceType));
	pItem->nType = GW_ItemSlotBase::GW_ItemSlotType::CASH;
	pItem->bIsCash = true;
	pItem->Load(liCashItemSN);
	pItem->nPOS = GW_ItemSlotBase::LOCK_POS;
	pItem->Save(nCharacterID);

	OutPacket oPacket;
	oPacket.Encode2((short)CenterResultPacketType::CashItemResult);
	oPacket.Encode4(uClientSocketSN);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode2(CenterCashItemRequestType::eMoveCashItemStoLRequest);
	oPacket.Encode1(0); //FailedReason
	oPacket.Encode8(liCashItemSN);
	oPacket.Encode1(nType);
	cashItemInfo.Encode(&oPacket);
	oPacket.Encode4(0);
	oPacket.Encode4(0);
	pSrv->SendPacket(&oPacket);
}

void CashItemDBAccessor::PostMoveLockerToSlotRequest(SocketBase * pSrv, int uClientSocketSN, int nCharacterID, void * iPacket_)
{
	InPacket *iPacket = (InPacket*)iPacket_;
	int nAccountID = iPacket->Decode4();
	long long int liCashItemSN = iPacket->Decode8();
	OutPacket oPacket;
	oPacket.Encode2((short)CenterResultPacketType::CashItemResult);
	oPacket.Encode4(uClientSocketSN);
	oPacket.Encode4(nCharacterID);

	GW_CashItemInfo cashItemInfo;
	cashItemInfo.Load(liCashItemSN);
	GA_Character characterData;
	characterData.Load(nCharacterID);

	ZSharedPtr<GW_ItemSlotBase> pItem;
	pItem.reset(GW_ItemSlotBase::CreateItem(cashItemInfo.nGWItemSlotInstanceType));
	pItem->nType = GW_ItemSlotBase::GW_ItemSlotType::CASH;
	pItem->bIsCash = true;
	pItem->Load(liCashItemSN);
	auto nPOS = characterData.FindEmptySlotPosition((int)pItem->nType);
	oPacket.Encode2((short)CenterCashItemRequestType::eMoveCashItemLtoSRequest);
	if (!nPOS)
	{
		oPacket.Encode1(1);
		pSrv->SendPacket(&oPacket);
		return;
	}
	pItem->nPOS = nPOS;
	pItem->Save(nCharacterID);
	cashItemInfo.bLocked = false;
	cashItemInfo.Save();

	oPacket.Encode1(0);
	oPacket.Encode8(pItem->liItemSN);
	//oPacket.Encode1(1);
	oPacket.Encode2(nPOS);
	pItem->RawEncode(&oPacket);
	oPacket.Encode4(0);
	oPacket.Encode4(0);
	pSrv->SendPacket(&oPacket);
}

void CashItemDBAccessor::PostExpireCashItemRequest(SocketBase * pSrv, int uClientSocketSN, int nCharacterID, void * iPacket_)
{
	InPacket *iPacket = (InPacket*)iPacket_;
	int nSize = iPacket->Decode1(), nTI = 0;

	GW_ItemSlotBase *pItem;
	GW_ItemSlotEquip gwEqp;
	GW_ItemSlotBundle gwBundle;
	gwBundle.nType = GW_ItemSlotBase::CASH;
	gwEqp.bIsCash = gwBundle.bIsCash = true;

	OutPacket oPacket;
	oPacket.Encode2(CenterResultPacketType::CashItemResult);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode2(CenterCashItemRequestType::eExpireCashItemRequest);
	oPacket.Encode1((char)nSize);

	for (int i = 0; i < nSize; ++i)
	{
		pItem = &gwEqp;
		nTI = iPacket->Decode1();
		if (nTI == GW_ItemSlotBase::CASH)
			pItem = &gwBundle;

		pItem->Load(iPacket->Decode8());
		pItem->Save(nCharacterID, false, true);
		oPacket.Encode1((char)nTI);
		oPacket.Encode8(pItem->liCashItemSN);
	}

	pSrv->SendPacket(&oPacket);
}