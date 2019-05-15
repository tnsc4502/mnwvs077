#pragma once

class ClientSocket;
class OutPacket;
class InPacket;
class AsyncScheduler;
struct GA_Character;
struct GW_FuncKeyMapped;

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
		Recv_OnCashItemReqIncItemSlot = 0x05,
		Recv_OnCashItemReqMoveItemToSlot = 0x0C,
		Recv_OnCashItemReqMoveItemToLocker = 0x0D,
		Recv_OnCashItemReqPackage = 0x24,

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
		Send_OnCashItemResCoupleDone = 0x88,
		Send_OnCashItemResCoupleFailed = 0x89,
		Send_OnCashItemResBuyPackageDone = 0x8A,
		Send_OnCashItemResBuyPackageFailed = 0x8B,
		Send_OnCashItemResGiftPackageDone = 0x8C,
		Send_OnCashItemResGiftPackageFailed = 0x8D,
		Send_OnCashItemResBuyNormalDone = 0x8E,
		Send_OnCashItemResBuyNormalFailed = 0x8F,
		Send_OnCashItemResApplyWishListEventDone = 0x90,
		Send_OnCashItemResApplyWishListEvenFailed = 0x91,
		Send_OnCashItemResFriendShopDone = 0x92,
		Send_OnCashItemResFirendShopFailed = 0x93,
		Send_OnCashItemResPurchaseRecord = 0x9D,
		Send_OnCashItemResPurchaseRecordFailed = 0x9E,
	};

	int m_nChannelID, nNexonCash = 0, nMaplePoint = 0;
	ClientSocket *m_pSocket;
	GA_Character *m_pCharacterData;
	GW_FuncKeyMapped *m_pFuncKeyMapped;
	AsyncScheduler *m_pUpdateTimer;

public:
	User(ClientSocket *pSocket, InPacket *iPacket);
	~User();
	int GetUserID() const;
	static User * FindUser(int nUserID);
	GA_Character* GetCharacterData();
	void SendPacket(OutPacket *oPacket);
	void OnPacket(InPacket *iPacket);
	void OnMigrateOutCashShop();
	void Update();

	void OnUserCashItemRequest(InPacket *iPacket);
	void OnQueryCashRequest();
	void ValidateState();

	void OnCenterCashItemResult(int nType, InPacket *iPacket);
	void OnCenterResLoadLockerDone(InPacket *iPacket);
	void OnCenterResBuyDone(InPacket *iPacket);
	void OnCenterUpdateCashDone(InPacket *iPacket);
	void OnCenterMoveItemToSlotDone(InPacket *iPacket);
	void OnCenterMoveItemToLockerDone(InPacket *iPacket);

	void OnRequestCenterLoadLocker();
	void OnRequestCenterUpdateCash();
	void OnRequestBuyCashItem(InPacket *iPacket);
	void OnRequestMoveItemToSlot(InPacket *iPacket);
	void OnRequestMoveItemToLocker(InPacket *iPacket);
};