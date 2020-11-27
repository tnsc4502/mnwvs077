#pragma once
#include "..\WvsLib\Memory\ZMemory.h"
#include <vector>
#include <string>
#include <map>

class ClientSocket;
class OutPacket;
class InPacket;
class AsyncScheduler;
struct CSCommodity;
struct GA_Character;
struct GW_FuncKeyMapped;
struct GW_GiftList;
struct GW_WishList;

class User
{
	enum CashItemPurchaseResult
	{
		Res_Success = 0x01,
		Res_Failed = 0x00
	};

	enum CashItemRequest
	{
		Recv_OnCashItemReqCoupon = 0x00,
		Recv_OnCashItemReqBuy = 0x03,
		Recv_OnCashItemReqGift = 0x04,
		Recv_OnCashItemReqSetWish = 0x05,
		Recv_OnCashItemReqIncItemSlot = 0x06,
		Recv_OnCashItemReqMoveItemToSlot = 0x0C,
		Recv_OnCashItemReqMoveItemToLocker = 0x0D,
		Recv_OnCashItemReqBuyCashPackage = 0x1C,
		Recv_OnCashItemReqGiftCashPackage = 0x1D,

		Send_OnCashItemResLimitGoodsCountChanged = 0x4C,
		Send_OnCashItemResLoadLockerDone = 46,
		Send_OnCashItemResLoadLockerFailed = 47,
		Send_OnCashItemResLoadGiftDone = 48,
		Send_OnCashItemResLoadGiftFailed = 49,
		Send_OnCashItemResLoadWishDone = 50,
		Send_OnCashItemResLoadWishFailed = 51,
		Send_OnCashItemResSetWishDone = 52,
		Send_OnCashItemResSetWishFailed = 53,
		Send_OnCashItemResBuyDone = 54,
		Send_OnCashItemResBuyFailed = 55,
		Send_OnCashItemResUseCouponDone = 56,
		/*CS_OnCashItemResGiftCouponDone     =     57,
		Send_OnCashItemResGiftCouponFaile     =     58, */
		Send_OnCashItemResGiftDone = 61,
		Send_OnCashItemResGiftFailed = 62,
		Send_OnCashItemResIncSlotCountDone = 63,
		Send_OnCashItemResIncSlotCountFailed = 64,
		//CS_OnCashItemResIncTrunkCountDone     =     63, 
		//CS_OnCashItemResIncTrunkCountFailed     =     64, 
		/*CS_OnCashItemResIncCharacterSlotCountDone     =     0x65,
		Send_OnCashItemResIncCharacterSlotCountFailed     =     0x66, */
		//CS_OnCashItemResIncBuyCharacterCountDone     =     0x67, 
		//CS_OnCashItemResIncBuyCharacterCountFailed     =     0x68, 
		/*CS_OnCashItemResEnableEquipSlotExtDone     =     0x67,
		Send_OnCashItemResEnableEquipSlotExtFailed     =     0x68, */
		Send_OnCashItemResMoveStoLDone = 71,
		Send_OnCashItemResMoveStoLFailed = 72,
		Send_OnCashItemResMoveLtoSDone = 69,
		Send_OnCashItemResMoveLtoSFailed = 70,
		Send_OnCashItemResDestroyDone = 69,
		Send_OnCashItemResDestroyFailed = 70,
		Send_OnCashItemResExpireDone = 73,
		Send_OnCashItemResRebateDone = 74,
		Send_OnCashItemResRebateFailed = 75,
		Send_OnCashItemResCoupleDone = 98,
		Send_OnCashItemResCoupleFailed = 99,
		Send_OnCashItemResBuyPackageDone = 100,
		Send_OnCashItemResBuyPackageFailed = 101,
		Send_OnCashItemResGiftPackageDone = 102,
		Send_OnCashItemResGiftPackageFailed = 103,
		Send_OnCashItemResBuyNormalDone = 104,
		Send_OnCashItemResBuyNormalFailed = 105,
		Send_OnCashItemResApplyWishListEventDone = 106,
		Send_OnCashItemResApplyWishListEvenFailed = 107,
		Send_OnCashItemResFriendShopDone = 108,
		Send_OnCashItemResFirendShopFailed = 109,
		Send_OnCashItemResPurchaseRecord = 110,
		Send_OnCashItemResPurchaseRecordFailed = 111,
	};

	int m_nChannelID, m_nNexonCash = 0, m_nMaplePoint = 0;
	bool m_bTransferChannel = false;
	ClientSocket *m_pSocket;
	ZUniquePtr<GA_Character> m_pCharacterData;
	ZUniquePtr<GW_FuncKeyMapped> m_pFuncKeyMapped;
	ZUniquePtr<AsyncScheduler> m_pUpdateTimer;
	std::map<int, ZUniquePtr<GW_GiftList>> m_mGiftList;
	ZUniquePtr<GW_WishList> m_pWishList;

public:
	User(ClientSocket *pSocket, InPacket *iPacket);
	~User();

	//Basic Stuffs
	int GetUserID() const;
	int GetAccountID() const;
	static User * FindUser(int nUserID);
	ZUniquePtr<GA_Character>& GetCharacterData();
	void SendPacket(OutPacket *oPacket);
	void OnPacket(InPacket *iPacket);
	void OnMigrateOutCashShop();
	void Update();

	void OnUserCashItemRequest(InPacket *iPacket);
	void ValidateState();

	//CenterRes
	void OnCenterCashItemResult(int nType, InPacket *iPacket);
	void OnCenterResLoadLockerDone(InPacket *iPacket);
	void OnCenterResBuyDone(InPacket *iPacket);
	void OnCenterGiftCashItemDone(InPacket *iPacket);
	void OnCenterBuyCashPackageDone(InPacket *iPacket);
	void OnCenterUpdateCashDone(InPacket *iPacket);
	void OnCenterMoveItemToSlotDone(InPacket *iPacket);
	void OnCenterMoveItemToLockerDone(InPacket *iPacket);
	void OnCenterGiftListResult(InPacket *iPacket);
	void OnCenterWishListResult(InPacket *iPacket, bool bLoad = false);

	//CenterReq
	void OnQueryCashRequest();
	void OnRequestCenterLoadLocker();
	void OnRequestCenterUpdateCash();
	void RequestLoadGiftList();
	void RequestLoadWishList();
	void OnMemoRequest(InPacket *iPacket);

	//CashItemReq
	bool EncodeBuyCashItem(OutPacket *oPacket, const std::vector<const CSCommodity *>& apCommodity, int nRequestType, int nChargeType, int nPrice, bool bGift = false, const std::string &sReceiver = "", const std::string& sMemo = "");
	void OnRequestBuyCashItem(InPacket *iPacket);
	void OnRequestMoveItemToSlot(InPacket *iPacket);
	void OnRequestMoveItemToLocker(InPacket *iPacket);
	void OnRequestCashItemGift(InPacket *iPacket);
	void OnRequestBuyCashPackage(InPacket *iPacket);
	void OnRequestGiftCashPackage(InPacket *iPacket);
	void OnRequestSetWishList(InPacket *iPacket, bool bLoad = false);
};