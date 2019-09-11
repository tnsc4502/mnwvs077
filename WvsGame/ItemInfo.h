#pragma once
#include "StateChangeItem.h"
#include "EquipItem.h"
#include "BundleItem.h"
#include "UpgradeItem.h"
#include "PortalScrollItem.h"
#include "PetFoodItem.h"
#include "MobSummonItem.h"
#include "TamingMobFoodItem.h"
#include "BridleItem.h"
#include "SkillLearnItem.h"
#include "PortableChairItem.h"
#include "CashItem.h"

#include <vector>

class User;
struct GA_Character;
struct GW_ItemSlotBase;

class ItemInfo
{
private:
	bool m_bInitialized = false;

public:
	enum ItemAttribute
	{
		eLocked = 0x01,
		eTradeBlockAfterEquip = 0x02,
		eUntradable = 0x08,
		eNotSale = 0x10,
		eExpireOnLogout = 0x20,
		ePickUpBlock = 0x40,
		eOnly = 0x80,
		eAccountSharable = 0x1000,
		eQuest = 0x200,
		eTradeBlock = 0x400,
		eAccountShareTag = 0x800,
		eMobHP = 0x1000
	};

	enum ItemVariationOption
	{
		ITEMVARIATION_NONE, 
		ITEMVARIATION_BETTER,
		ITEMVARIATION_NORMAL,
		ITEMVARIATION_GREAT,
		ITEMVARIATION_GACHAPON
	};

	enum CashItemType
	{
		CashItemType_None = 0x00,
		CashItemType_HairStyleCoupon = 0x01,
		CashItemType_FaceStyleCoupon = 0x02,
		CashItemType_SkinCareCoupon = 0x03,
		CashItemType_PersonalStore = 0x04,
		CashItemType_WaterOfLife = 0x05,
		CashItemType_Emotion = 0x06,
		CashItemType_Charm = 0x07,
		CashItemType_Effect = 0x09,
		CashItemType_ThrowingStar = 0x0A,
		CashItemType_EntrustedShop = 0x0B,
		CashItemType_SpeakerChannel = 0x0C,
		CashItemType_SpeakerWorld = 0x0D,
		CashItemType_Weather = 0x0E,
		CashItemType_SetPetName = 0x0F,
		CashItemType_MessageBox = 0x10,
		CashItemType_MoneyPocket = 0x11, 
		CashItemType_JukeBox = 0x12,
		CashItemType_SendMemo = 0x13,
		CashItemType_MapTransfer = 0x14,
		CashItemType_StatChange = 0x15,
		CashItemType_SkillChange = 0x16,
		CashItemType_SetItemName = 0x17,
		CashItemType_ItemProtector = 0x18,
		CashItemType_Incubator = 0x19,
		CashItemType_PetSkill = 0x1A, 
		CashItemType_ShopScanner = 0x1B,
		CashItemType_ADBoard = 0x1C, 
		CashItemType_GachaponTicket = 0x1D,
		CashItemType_PetFood = 0x1E,
		CashItemType_Morph = 0x1F, 
		CashItemType_WeddingTicket = 0x20,
		CashItemType_DeliveryTicket = 0x22,
		CashItemType_EvolutionRock = 0x23,
		CashItemType_AvatarMegaphone = 0x24,
		CashItemType_MapleTV = 0x28,
		CashItemType_MapleSoleTV = 0x29,
		CashItemType_MapleLoveTV = 0x2A,
		CashItemType_MegaTV = 0x2B,
		CashItemType_MegaSoleTV = 0x2C,
		CashItemType_MegaLoveTV = 0x2D,
		CashItemType_NameChange = 0x2E,
		CashItemType_MembershipCoupon = 0x2F,
		CashItemType_TransferWorld = 0x32,
	};

	ItemInfo();
	~ItemInfo();

	static ItemInfo* GetInstance();
	void Initialize();
	void LoadItemSellPriceByLv();
	void IterateMapString(void *dataNode);
	void IterateItemString(void *dataNode);
	void IterateEquipItem(void *dataNode);
	void IterateBundleItem();
	void IteratePetItem();
	void IterateCashItem();
	void RegisterSpecificItems();
	void RegisterNoRollbackItem();
	void RegisterSetHalloweenItem();

	void RegisterEquipItemInfo(EquipItem* pEqpItem, int nItemID, void* pProp);
	void RegisterUpgradeItem(int nItemID, void *pProp);
	void RegisterPortalScrollItem(int nItemID, void *pProp);
	void RegisterMobSummonItem(int nItemID, void *pProp);
	void RegisterPetFoodItem(int nItemID, void *pProp);
	void RegisterTamingMobFoodItem(int nItemID, void *pProp);
	void RegisterBridleItem(int nItemID, void *pProp);
	void RegisterPortableChairItem(int nItemID, void *pProp);
	void RegisterSkillLearnItem(int nItemID, void *pProp);
	void RegisterStateChangeItem(int nItemID, void *pProp);

	EquipItem* GetEquipItem(int nItemID);
	BundleItem* GetBundleItem(int nItemID);
	UpgradeItem* GetUpgradeItem(int nItemID);
	PortalScrollItem* GetPortalScrollItem(int nItemID);

	MobSummonItem* GetMobSummonItem(int nItemID);
	PetFoodItem* GetPetFoodItem(int nItemID);
	TamingMobFoodItem* GetTamingMobFoodItem(int nItemID);
	BridleItem* GetBridleItem(int nItemID);
	SkillLearnItem* GetSkillLearnItem(int nItemID);
	PortableChairItem* GetPortableChairItem(int nItemID);
	StateChangeItem* GetStateChangeItem(int nItemID);
	CashItem* GetCashItem(int nItemID);

	static int GetItemSlotType(int nItemID);
	static bool IsTreatSingly(int nItemID, long long int liExpireDate);
	static bool IsRechargable(int nItemID);
	int ConsumeOnPickup(int nItemID);
	bool ExpireOnLogout(int nItemID);
	int GetBulletPAD(int nItemID);
	long long int GetItemDateExpire(const std::string& sDate);
	const std::string& GetItemName(int nItemID);
	bool IsAbleToEquip(int nGender, int nLevel, int nJob, int nSTR, int nDEX, int nINT, int nLUK, int nPOP, GW_ItemSlotBase* pPetItem, int nItemID);
	bool IsNotSaleItem(int nItemID);
	bool IsOnlyItem(int nItemID);
	bool IsTradeBlockItem(int nItemID);
	bool IsQuestItem(int nItemID);
	static bool IsWeapon(int nItemID);
	static bool IsCoat(int nItemID);
	static bool IsCape(int nItemID);
	static bool IsPants(int nItemID);
	static bool IsHair(int nItemID);
	static bool IsFace(int nItemID);
	static bool IsShield(int nItemID);
	static bool IsShoes(int nItemID);
	static bool IsLongcoat(int nItemID);
	static bool IsCap(int nItemID);
	static bool IsPet(int nItemID);
	static int GetCashSlotItemType(int nItemID);
	static int GetConsumeCashItemType(int nItemID);
#ifdef _WVSGAME
	static int GetWeaponMastery(GA_Character* pCharacter, int nWeaponID, int nSkillID, int nAttackType, int *pnACCInc, int *pnPADInc);
	static int GetWeaponType(int nItemID);
	static int GetCriticalSkillLevel(GA_Character* pCharacter, int nWeaponID, int nAttackType, int *pnProp, int *pnParam);
#endif
	GW_ItemSlotBase* GetItemSlot(int nItemID, ItemVariationOption enOption);

private:
	std::map<int, EquipItem*> m_mEquipItem;
	std::map<int, BundleItem*> m_mBundleItem;
	std::map<int, UpgradeItem*> m_mUpgradeItem;
	std::map<int, StateChangeItem*> m_mStateChangeItem;
	std::map<int, PortalScrollItem*> m_mPortalScrollItem;
	std::map<int, MobSummonItem*> m_mMobSummonItem;
	std::map<int, PetFoodItem*> m_mPetFoodItem;
	std::map<int, TamingMobFoodItem*> m_mTamingMobFoodItem;
	std::map<int, BridleItem*> m_mBridleItem;
	std::map<int, SkillLearnItem*> m_mSkillLearnItem;
	std::map<int, PortableChairItem*> m_mPortableChairItem;
	std::map<int, CashItem*> m_mCashItem;

	std::map<int, std::string> m_mItemString, m_mMapString;
	std::map<int, int> m_mItemSellPriceByLv;
	
	void LoadIncrementStat(BasicIncrementStat& refStat, void *pProp);
	void LoadAbilityStat(BasicAbilityStat& refStat, void *pProp);


	/*
	用於初始化物品的能力數值，其中v是預設數值，根據enOption來決定數值的偏差結果
	*/
	int GetVariation(int v, ItemVariationOption enOption);
};

