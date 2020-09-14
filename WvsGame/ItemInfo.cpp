#include "ItemInfo.h"
#include "SkillInfo.h"
#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_ItemSlotPet.h"
#include "..\Database\GA_Character.hpp"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\String\StringUtility.h"
#include "..\WvsLib\String\StringPool.h"

#include <windows.h>

ItemInfo::ItemInfo()
{
}


ItemInfo::~ItemInfo()
{
}

ItemInfo * ItemInfo::GetInstance()
{
	static ItemInfo* pInstance = new ItemInfo;
	return pInstance;
}

void ItemInfo::Initialize()
{
	IterateMapString(nullptr);
	LoadItemSellPriceByLv();
	WvsLogger::LogRaw("[ItemInfo::Initialize<IterateItemString>]On iterating all item names....\n");
	IterateItemString(nullptr);
	WvsLogger::LogRaw("[ItemInfo::Initialize<IterateItemString>]Item names are completely loaded.\n");

	static auto& eqpWz = stWzResMan->GetWz(Wz::Character);
	WvsLogger::LogRaw("[ItemInfo::Initialize<IterateEquipItem>]On iterating all equip items....\n");
	IterateEquipItem(&eqpWz);
	WvsLogger::LogRaw("[ItemInfo::Initialize<IterateEquipItem>]Equip items are completely loaded.\n");

	WvsLogger::LogRaw("[ItemInfo::Initialize<IterateBundleItem>]On iterating all bundle items....\n");
	IterateBundleItem();
	WvsLogger::LogRaw("[ItemInfo::Initialize<IterateBundleItem>]Bundle items are completely loaded.\n");
	//IterateCashItem();
	IteratePetItem();
	RegisterSpecificItems();
	RegisterNoRollbackItem();
	RegisterSetHalloweenItem();
	//stWzResMan->Unmount("./Game/Item.wz");
	//stWzResMan->ReleaseMemory();
	//WvsLogger::LogRaw("[ItemInfo::Initialize]釋放ItemInfo所有Wz記憶體[ReleaseMemory Done]....\n");
}

void ItemInfo::LoadItemSellPriceByLv()
{
	auto& info = stWzResMan->GetWz(Wz::Item)["ItemSellPriceStandard"]["400"];
	for (auto& lvl : info)
		m_mItemSellPriceByLv[atoi(lvl.GetName().c_str())] = (int)lvl;
}

void ItemInfo::IterateMapString(void *dataNode)
{
	static WzIterator Img[] = {
		WzResMan::GetInstance()->GetWz(Wz::String)["Map"]
	};
	if (dataNode == nullptr)
		for (auto& img : Img)
			IterateMapString((void*)&img);
	else
	{
		auto& dataImg = (*((WzIterator*)dataNode));
		for (auto& img : dataImg)
		{
			if (!isdigit(img.GetName()[0]) || atoi(img.GetName().c_str()) < 1000)
				IterateMapString((void*)&img);
			else
				m_mMapString[atoi(img.GetName().c_str())] = StringUtility::ConvertUTF8ToSystemEncoding(((std::string)img["mapName"]).c_str());
		}
	}
}

void ItemInfo::IterateItemString(void *dataNode)
{
	static WzIterator Img[] = { 
		stWzResMan->GetWz(Wz::String)["Eqp"], 
		stWzResMan->GetWz(Wz::String)["Etc"], 
		stWzResMan->GetWz(Wz::String)["Consume"], 
		stWzResMan->GetWz(Wz::String)["Ins"],
		stWzResMan->GetWz(Wz::String)["Cash"],
		stWzResMan->GetWz(Wz::String)["Pet"]
	};
	if (dataNode == nullptr)
		for (auto& img : Img)
			IterateItemString((void*)&img);
	else
	{
		auto& dataImg = (*((WzIterator*)dataNode));
		for (auto& img : dataImg)
		{
			if (!isdigit(img.GetName()[0]) || atoi(img.GetName().c_str()) < 1000)
				IterateItemString((void*)&img);

			else
				m_mItemString[atoi(img.GetName().c_str())] = StringUtility::ConvertUTF8ToSystemEncoding(((std::string)img["name"]).c_str());
		}
	}
}

void ItemInfo::IterateEquipItem(void *dataNode)
{
	auto& dataImg = (*((WzIterator*)dataNode));
	int nItemID = 0;
	
	for (auto& data : dataImg)
	{
		if (data.GetName() == "Hair" || data.GetName() == "Face" || data.GetName() == "Afterimage")
			continue;
		if (!isdigit(data.GetName()[0])) //展開資料夾
		{
			clock_t tStart = clock();
			IterateEquipItem((void*)(&data));
			//printf("%s loading : %.2fs\n", data.GetName().c_str(), (double)(clock() - tStart) / CLOCKS_PER_SEC);
		}
		else
		{
			nItemID = atoi(data.GetName().c_str());
			if (nItemID < 20000)
				continue;
			EquipItem* pNewEquip = AllocObj(EquipItem);
			pNewEquip->nItemID = nItemID;
			pNewEquip->sItemName = m_mItemString[nItemID];
			RegisterEquipItemInfo(pNewEquip, nItemID, (void*)&(data));
			m_mEquipItem[nItemID] = pNewEquip;
		}
	}
}

void ItemInfo::IterateBundleItem()
{
#undef max
	static WzIterator Img[] = { 
		stWzResMan->GetWz(Wz::Item)["Cash"], 
		stWzResMan->GetWz(Wz::Item)["Consume"], 
		stWzResMan->GetWz(Wz::Item)["Etc"], 
		stWzResMan->GetWz(Wz::Item)["Install"] 
	};
	for (auto& baseImg : Img)
	{
		for (auto& dir : baseImg)
		{
			for (auto& item : dir)
			{
				auto& infoImg = item["info"];
				int nItemID = atoi(item.GetName().c_str());
				BundleItem* pNewBundle = AllocObj( BundleItem );
				LoadAbilityStat(pNewBundle->abilityStat, (void*)&infoImg);
				if (pNewBundle->abilityStat.bCash)
					m_mCashItem.insert({ nItemID, AllocObj(CashItem) });

				pNewBundle->nItemID = nItemID;
				pNewBundle->sItemName = m_mItemString[nItemID];
				pNewBundle->nMaxPerSlot = infoImg["slotMax"];
				if (!pNewBundle->nMaxPerSlot)
					pNewBundle->nMaxPerSlot = 100;
				pNewBundle->dSellUnitPrice = (double)infoImg["unitPrice"];
				pNewBundle->nSellPrice = infoImg["price"];
				pNewBundle->nRequiredLEV = infoImg["reqLevel"];
				pNewBundle->nLevel = infoImg["lv"];
				if (pNewBundle->nSellPrice == 0)
					pNewBundle->nSellPrice = m_mItemSellPriceByLv[pNewBundle->nLevel];

				auto& mcType = infoImg["mcType"];
				if (mcType != infoImg.end())
					pNewBundle->nMCType = mcType;
				else
					pNewBundle->nMCType = -1;

				pNewBundle->nPAD = infoImg["incPAD"]; //飛鏢
				m_mBundleItem[nItemID] = pNewBundle;
				int nItemCategory = nItemID / 10000;
				void* pProp = (void*)&item;
				switch (nItemCategory)
				{
					case 200:
					case 201:
					case 202:
					case 205:
					case 221:
						RegisterStateChangeItem(nItemID, pProp);
						break;
					case 204:
						RegisterUpgradeItem(nItemID, pProp);
						break;
					case 203:
						RegisterPortalScrollItem(nItemID, pProp);
						break;
					case 210:
						RegisterMobSummonItem(nItemID, pProp);
						break;
					case 212:
						RegisterPetFoodItem(nItemID, pProp);
						break;
					case 226:
						RegisterTamingMobFoodItem(nItemID, pProp);
						break;
					case 227:
						RegisterBridleItem(nItemID, pProp);
						break;
					case 228:
					case 229:
						RegisterSkillLearnItem(nItemID, pProp);
						break;
					case 301:
						RegisterPortableChairItem(nItemID, pProp);
						break;
				}
			}
		}
	}
}

void ItemInfo::IteratePetItem()
{
	auto& img = stWzResMan->GetWz(Wz::Item)["Pet"];
	for (auto& item : img)
	{
		int nItemID = atoi(item.GetName().c_str());
		CashItem *pItem = AllocObj(CashItem);
		pItem->bIsPet = true;
		m_mCashItem.insert({ nItemID, pItem });
	}
	
}

void ItemInfo::IterateCashItem()
{
	auto& img = stWzResMan->GetWz(Wz::Item)["Cash"];
	for (auto& subImg : img)
	{
		for (auto& item : subImg)
		{
			int nItemID = atoi(item.GetName().c_str());
			//std::cout << "Item = " << nItemID << std::endl;
			CashItem *pItem = AllocObj(CashItem);
			m_mCashItem.insert({ nItemID, pItem });
		}
	}
}

void ItemInfo::RegisterSpecificItems()
{
}

void ItemInfo::RegisterNoRollbackItem()
{
}

void ItemInfo::RegisterSetHalloweenItem()
{
}

void ItemInfo::RegisterEquipItemInfo(EquipItem * pEqpItem, int nItemID, void * pProp)
{
#undef max
	auto& infoImg = (*((WzIterator*)pProp))["info"];

	LoadIncrementStat(pEqpItem->incStat, (void*)&infoImg);
	LoadAbilityStat(pEqpItem->abilityStat, (void*)&infoImg);

	if (pEqpItem->abilityStat.bCash)
		m_mCashItem.insert({ nItemID, AllocObj(CashItem) });

	pEqpItem->nItemID = nItemID;
	pEqpItem->nrSTR = infoImg["reqSTR"];
	pEqpItem->nrDEX = infoImg["reqDEX"];
	pEqpItem->nrINT = infoImg["reqINT"];
	pEqpItem->nrLUK = infoImg["reqLUK"];
	pEqpItem->nrPOP = infoImg["reqPOP"];
	pEqpItem->nrJob = infoImg["reqJob"];
	pEqpItem->nrLevel = infoImg["reqLevel"];
	pEqpItem->nRUC = infoImg["tuc"];
	//pEqpItem->nrMobLevel = infoImg["reqMobLevel"];
	pEqpItem->nSellPrice = infoImg["price"];
	pEqpItem->nKnockBack = infoImg["knockback"];
	pEqpItem->nIncRMAF = infoImg["incRMAF"];
	pEqpItem->nIncRMAI = infoImg["incRMAI"];
	pEqpItem->nIncRMAL = infoImg["incRMAL"];
	pEqpItem->nElemDefault = infoImg["elemDefault"];
	pEqpItem->nCuttable = (int)infoImg["tradeAvailable"];
	pEqpItem->dwPetAbilityFlag = 0;
	pEqpItem->dRecovery = std::max(1.0, (double)infoImg["recovery"]);
	if (nItemID / 10000 == 181)
	{
		if ((int)infoImg["pickupItem"] == 1)
			pEqpItem->dwPetAbilityFlag |= 0x1;

		if ((int)infoImg["longRange"] == 1)
			pEqpItem->dwPetAbilityFlag |= 0x2;

		if ((int)infoImg["pickupOthers"] == 1)
			pEqpItem->dwPetAbilityFlag |= 0x10;

		if ((int)infoImg["consumeHP"] == 1)
			pEqpItem->dwPetAbilityFlag |= 0x20;

		if ((int)infoImg["consumeMP"] == 1)
			pEqpItem->dwPetAbilityFlag |= 0x40;
	}
}

void ItemInfo::RegisterUpgradeItem(int nItemID, void * pProp)
{
	auto& infoImg = (*((WzIterator*)pProp))["info"];
	UpgradeItem *pNewUpgradeItem = AllocObj(UpgradeItem);

	pNewUpgradeItem->nItemID = nItemID;
	LoadIncrementStat(pNewUpgradeItem->incStat, (void*)&infoImg);
	pNewUpgradeItem->nSuccessRate = infoImg["success"];
	pNewUpgradeItem->nCursedRate = infoImg["cursed"];
	m_mUpgradeItem[nItemID] = pNewUpgradeItem;
}

void ItemInfo::RegisterPortalScrollItem(int nItemID, void * pProp)
{
	PortalScrollItem *pNewPortalScrollItem = AllocObj(PortalScrollItem);
	pNewPortalScrollItem->nItemID = nItemID;
	auto& specImg = (*((WzIterator*)pProp))["spec"];
	for (auto& effect : specImg)
		pNewPortalScrollItem->spec.insert({ effect.GetName(), (int)effect });
	m_mPortalScrollItem[nItemID] = pNewPortalScrollItem;
}

void ItemInfo::RegisterMobSummonItem(int nItemID, void * pProp)
{
	auto& infoImg = (*((WzIterator*)pProp))["info"];
	MobSummonItem *pNewMobSummonItem = AllocObj(MobSummonItem);
	pNewMobSummonItem->nItemID = nItemID;
	pNewMobSummonItem->nType = infoImg["type"];
	auto& mobImg = (*((WzIterator*)pProp))["mob"];
	for (auto& mob : mobImg) 
		pNewMobSummonItem->lMob.push_back({ atoi(((std::string)mob["id"]).c_str()), (int)(mob["prob"]) });
	m_mMobSummonItem[nItemID] = pNewMobSummonItem;
}

void ItemInfo::RegisterPetFoodItem(int nItemID, void * pProp)
{
	auto& specImg = (*((WzIterator*)pProp))["spec"];
	PetFoodItem *pNewFoodItem = AllocObj(PetFoodItem);
	pNewFoodItem->nItemID = nItemID;
	pNewFoodItem->niRepleteness = specImg["inc"];
	for (auto& petID : specImg)
		if (isdigit(petID.GetName()[0]))
			pNewFoodItem->ldwPet.push_back((int)petID);
	m_mPetFoodItem[nItemID] = pNewFoodItem;
}

void ItemInfo::RegisterTamingMobFoodItem(int nItemID, void * pProp)
{
	auto& specImg = (*((WzIterator*)pProp))["spec"];
	TamingMobFoodItem *pNewTamingMobFoodItem = AllocObj(TamingMobFoodItem);
	pNewTamingMobFoodItem->nItemID = nItemID;
	pNewTamingMobFoodItem->niFatigue = specImg["incFatigue"];
	m_mTamingMobFoodItem[nItemID] = pNewTamingMobFoodItem;
}

void ItemInfo::RegisterBridleItem(int nItemID, void * pProp)
{
	auto& infoImg = (*((WzIterator*)pProp))["info"];
	BridleItem *pNewBridleItem = AllocObj(BridleItem);
	pNewBridleItem->nItemID = nItemID;
	pNewBridleItem->dwTargetMobID = infoImg["mob"];
	pNewBridleItem->nCreateItemID = infoImg["create"];
	pNewBridleItem->nBridleProp = infoImg["bridleProp"];
	pNewBridleItem->nBridleMsgType = infoImg["bridleMsgType"];
	pNewBridleItem->nBridleHP = infoImg["mobHP"];
	pNewBridleItem->nUseDelay = infoImg["useDelay"];
	pNewBridleItem->dBridlePropChg = (double)infoImg["bridlePropChg"];
	m_mBridleItem[nItemID] = pNewBridleItem;
}

void ItemInfo::RegisterPortableChairItem(int nItemID, void * pProp)
{
	auto& infoImg = (*((WzIterator*)pProp))["info"];
	PortableChairItem *pNewPortableChairItem = AllocObj(PortableChairItem);
	pNewPortableChairItem->nItemID = nItemID;
	pNewPortableChairItem->nReqLevel = infoImg["reqLevel"];
	pNewPortableChairItem->nPortableChairRecoveryRateMP = infoImg["recoveryMP"];
	pNewPortableChairItem->nPortableChairRecoveryRateHP = infoImg["recoveryHP"];
	m_mPortableChairItem[nItemID] = pNewPortableChairItem;
}

void ItemInfo::RegisterSkillLearnItem(int nItemID, void * pProp)
{
	auto& infoImg = (*((WzIterator*)pProp))["info"];
	SkillLearnItem *pNewSkillLearnItem = AllocObj(SkillLearnItem);
	pNewSkillLearnItem->nItemID = nItemID;
	pNewSkillLearnItem->nMasterLevel = infoImg["masterLevel"];
	pNewSkillLearnItem->nSuccessRate = infoImg["success"];
	if (nItemID / 10000 == 229)
		pNewSkillLearnItem->nReqLevel = infoImg["reqSkillLevel"];
	auto& skillImg = infoImg["skill"];
	for (auto& skill : skillImg)
		pNewSkillLearnItem->aSkill.push_back((int)skill);
	m_mSkillLearnItem[nItemID] = pNewSkillLearnItem;
}

void ItemInfo::RegisterStateChangeItem(int nItemID, void * pProp)
{
	StateChangeItem *pNewStateChangeItem = AllocObj(StateChangeItem);
	pNewStateChangeItem->nItemID = nItemID;
	auto& specImg = (*((WzIterator*)pProp))["spec"];
	for(auto& effect : specImg)
		pNewStateChangeItem->spec.insert({ effect.GetName(), (int)effect });
	m_mStateChangeItem[nItemID] = pNewStateChangeItem;
}

EquipItem * ItemInfo::GetEquipItem(int nItemID)
{
	auto findIter = m_mEquipItem.find(nItemID);
	return (findIter != m_mEquipItem.end() ? findIter->second : nullptr);
}

StateChangeItem * ItemInfo::GetStateChangeItem(int nItemID)
{
	auto findIter = m_mStateChangeItem.find(nItemID);
	return (findIter != m_mStateChangeItem.end() ? findIter->second : nullptr);
}

CashItem * ItemInfo::GetCashItem(int nItemID)
{
	auto findIter = m_mCashItem.find(nItemID);
	return (findIter != m_mCashItem.end() ? findIter->second : nullptr);
}

BundleItem * ItemInfo::GetBundleItem(int nItemID)
{
	auto findIter = m_mBundleItem.find(nItemID);
	return (findIter != m_mBundleItem.end() ? findIter->second : nullptr);
}

UpgradeItem * ItemInfo::GetUpgradeItem(int nItemID)
{
	auto findIter = m_mUpgradeItem.find(nItemID);
	return (findIter != m_mUpgradeItem.end() ? findIter->second : nullptr);
}

PortalScrollItem * ItemInfo::GetPortalScrollItem(int nItemID)
{
	auto findIter = m_mPortalScrollItem.find(nItemID);
	return (findIter != m_mPortalScrollItem.end() ? findIter->second : nullptr);
}

MobSummonItem * ItemInfo::GetMobSummonItem(int nItemID)
{
	auto findIter = m_mMobSummonItem.find(nItemID);
	return (findIter != m_mMobSummonItem.end() ? findIter->second : nullptr);
}

PetFoodItem * ItemInfo::GetPetFoodItem(int nItemID)
{
	auto findIter = m_mPetFoodItem.find(nItemID);
	return (findIter != m_mPetFoodItem.end() ? findIter->second : nullptr);
}

TamingMobFoodItem * ItemInfo::GetTamingMobFoodItem(int nItemID)
{
	auto findIter = m_mTamingMobFoodItem.find(nItemID);
	return (findIter != m_mTamingMobFoodItem.end() ? findIter->second : nullptr);
}

BridleItem * ItemInfo::GetBridleItem(int nItemID)
{
	auto findIter = m_mBridleItem.find(nItemID);
	return (findIter != m_mBridleItem.end() ? findIter->second : nullptr);
}

SkillLearnItem * ItemInfo::GetSkillLearnItem(int nItemID)
{
	auto findIter = m_mSkillLearnItem.find(nItemID);
	return (findIter != m_mSkillLearnItem.end() ? findIter->second : nullptr);
}

PortableChairItem * ItemInfo::GetPortableChairItem(int nItemID)
{
	auto findIter = m_mPortableChairItem.find(nItemID);
	return (findIter != m_mPortableChairItem.end() ? findIter->second : nullptr);
}

int ItemInfo::GetItemSlotType(int nItemID)
{
	return nItemID / 1000000;
}

bool ItemInfo::IsTreatSingly(int nItemID, long long int liExpireDate)
{
	int nItemHeader = GetItemSlotType(nItemID);
	return ((nItemHeader != 2 && nItemHeader != 3 && nItemHeader != 4 && nItemHeader != 5)
		|| IsRechargable(nItemID)
		/*|| liExpireDate != 0*/);
}

bool ItemInfo::IsRechargable(int nItemID)
{
	return nItemID / 10000 == 207 || nItemID / 10000 == 233;
}

int ItemInfo::ConsumeOnPickup(int nItemID)
{
	StateChangeItem* pItem = nullptr;
	if (nItemID / 1000000 == 2 && (pItem = GetStateChangeItem(nItemID)) != nullptr)
	{
		for (auto& p : pItem->spec)
			if (p.first == "consumeOnPickup")
				return p.second;
	}
	return 0;
}

bool ItemInfo::ExpireOnLogout(int nItemID)
{
	if (nItemID / 1000000 != 5)
		if (nItemID / 1000000 == 1)
		{
			auto pItem = GetEquipItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eExpireOnLogout) != 0;
		}
		else
		{
			auto pItem = GetBundleItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eExpireOnLogout) != 0;
		}
	return false;
}

int ItemInfo::GetBulletPAD(int nItemID)
{
	auto pItem = GetBundleItem(nItemID);
	if (pItem == nullptr)
		return 0;
	return pItem->nPAD;
}

long long int ItemInfo::GetItemDateExpire(const std::string & sDate)
{
	std::string sYear = sDate.substr(4);
	std::string sMonth = sDate.substr(4, 2);
	std::string sDay = sDate.substr(6, 2);
	std::string sHour = sDate.substr(8, 2);
	SYSTEMTIME sysTime;
	sysTime.wYear = atoi(sYear.c_str());
	sysTime.wMonth = atoi(sMonth.c_str());
	sysTime.wDay = atoi(sDay.c_str());
	sysTime.wHour = atoi(sHour.c_str());
	sysTime.wMilliseconds = 0;
	sysTime.wSecond = 0;
	sysTime.wMinute = 0;
	sysTime.wDayOfWeek = 0;
	FILETIME ft;
	SystemTimeToFileTime(&sysTime, &ft);
	return 0;
}

const std::string & ItemInfo::GetItemName(int nItemID)
{
	static std::string strEmpty = "";
	auto findResult = m_mItemString.find(nItemID);
	return (findResult == m_mItemString.end() ? strEmpty : findResult->second);
}

bool ItemInfo::IsAbleToEquip(int nGender, int nLevel, int nJob, int nSTR, int nDEX, int nINT, int nLUK, int nPOP, GW_ItemSlotBase * pPetItem, int nItemID)
{
	return false;
}

bool ItemInfo::IsNotSaleItem(int nItemID)
{
	int nTI = GetItemSlotType(nItemID);
	if (nTI != 5)
		if (nTI == 1)
		{
			auto pItem = GetEquipItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eNotSale) != 0;
		}
		else
		{
			auto pItem = GetBundleItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eNotSale) != 0;
		}
	return false;
}

bool ItemInfo::IsOnlyItem(int nItemID)
{
	int nTI = GetItemSlotType(nItemID);
	if (nTI != 5)
		if (nTI == 1)
		{
			auto pItem = GetEquipItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eOnly) != 0;
		}
		else
		{
			auto pItem = GetBundleItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eOnly) != 0;
		}
	return false;
}

bool ItemInfo::IsTradeBlockItem(int nItemID)
{
	int nTI = GetItemSlotType(nItemID);
	if (nTI != 5)
		if (nTI == 1)
		{
			auto pItem = GetEquipItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eTradeBlock) != 0;
		}
		else
		{
			auto pItem = GetBundleItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eTradeBlock) != 0;
		}
	return false;
}

bool ItemInfo::IsQuestItem(int nItemID)
{
	int nTI = GetItemSlotType(nItemID);
	if (nTI != 5)
		if (nTI == 1)
		{
			auto pItem = GetEquipItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eQuest) != 0;
		}
		else
		{
			auto pItem = GetBundleItem(nItemID);
			if (pItem != nullptr)
				return (pItem->abilityStat.nAttribute & ItemAttribute::eQuest) != 0;
		}
	return false;
}

bool ItemInfo::IsWeapon(int nItemID)
{
	return (nItemID >= 1210000 && nItemID < 1600000)
		|| nItemID / 10000 == 135;
}

bool ItemInfo::IsCoat(int nItemID)
{
	return nItemID / 10000 == 104;
}

bool ItemInfo::IsCape(int nItemID)
{
	return nItemID / 10000 == 100;
}

bool ItemInfo::IsPants(int nItemID)
{
	return nItemID / 10000 == 106;
}

bool ItemInfo::IsHair(int nItemID)
{
	return nItemID >= 30000 && nItemID <= 49999;
}

bool ItemInfo::IsFace(int nItemID)
{
	return nItemID >= 20000 && nItemID <= 29999;
}

bool ItemInfo::IsShield(int nItemID)
{
	return nItemID / 10000 == 109;
}

bool ItemInfo::IsShoes(int nItemID)
{
	return nItemID / 10000 == 107;
}

bool ItemInfo::IsLongcoat(int nItemID)
{
	return nItemID / 10000 == 105;
}

bool ItemInfo::IsCap(int nItemID)
{
	return nItemID / 10000 == 110;
}

bool ItemInfo::IsPet(int nItemID)
{
	int nTI = GetItemSlotType(nItemID);
	int nPrefix = nItemID / 1000;
	return nTI == GW_ItemSlotBase::CASH && (nPrefix % 10 == 0);
}

int ItemInfo::GetCashSlotItemType(int nItemID)
{
	switch (nItemID / 10000)
	{
		case 500:
			return 8; //Unable to find an item with ID 500xxxx?
		case 501:
			return CashItemType::CashItemType_Effect;
		case 502:
			return CashItemType::CashItemType_ThrowingStar;
		case 503:
			return CashItemType::CashItemType_EntrustedShop;
		case 507:
			if (nItemID % 10000 / 1000 == 1)
				return CashItemType::CashItemType_SpeakerChannel;
			else if (nItemID % 10000 / 1000 == 2)
				return CashItemType::CashItemType_SpeakerWorld;
			else if (nItemID % 10000 / 1000 == 3)
				return CashItemType::CashItemType_SpeakerHeart;
			else if (nItemID % 10000 / 1000 == 4)
				return CashItemType::CashItemType_SpeakerSkull;
			else
			{
				if (nItemID % 10000 / 1000 != 5)
					return CashItemType::CashItemType_None;
				if (nItemID % 10)
				{
					switch (nItemID % 10)
					{
					case 1:
						return CashItemType::CashItemType_MapleSoleTV;
					case 2:
						return CashItemType::CashItemType_MapleLoveTV;
					case 3:
						return CashItemType::CashItemType_MegaTV;
					case 4:
						return CashItemType::CashItemType_MegaSoleTV;
					default:
						return nItemID % 10 != 5 ?
							CashItemType::CashItemType_None : CashItemType::CashItemType_MegaLoveTV;
					}
				}
				else
					return CashItemType::CashItemType_MapleTV;
			}
		case 512:
			return CashItemType::CashItemType_Weather;
		case 517:
			return nItemID % 10000 != 0 ? 
				CashItemType::CashItemType_None : CashItemType::CashItemType_SetPetName;
		case 508:
			return CashItemType::CashItemType_MessageBox;
		case 520:
			return CashItemType::CashItemType_MoneyPocket;
		case 510:
			return CashItemType::CashItemType_JukeBox;
		case 509:
			return CashItemType::CashItemType_SendMemo;
		case 504:
			return CashItemType::CashItemType_MapTransfer;
		case 505:
			if (!(nItemID % 10))
				return CashItemType::CashItemType_StatChange;
			if (nItemID % 10 > 0 && nItemID % 10 <= 4)
				return CashItemType::CashItemType_SkillChange;
			return CashItemType::CashItemType_None;
		case 506:
			if (nItemID % 10)
			{
				if (nItemID % 10 == 1)
					return CashItemType::CashItemType_ItemProtector;
				else
					return nItemID % 10 != 2 ?
						CashItemType::CashItemType_None : CashItemType::CashItemType_Incubator;
			}
			else
				return CashItemType::CashItemType_SetItemName;
		case 519:
			return CashItemType::CashItemType_PetSkill;
		case 516:
			return CashItemType::CashItemType_Emotion;
		case 515:
			if (nItemID / 1000 == 5150 || nItemID / 1000 == 5151)
				return 1;
			if (nItemID / 1000 == 5152)
				return nItemID / 100 != 51520 ?
				CashItemType::CashItemType_None : CashItemType::CashItemType_FaceStyleCoupon;
			if (nItemID / 1000 == 5153)
				return CashItemType::CashItemType_SkinCareCoupon;
			else
				return nItemID / 1000 != 5154 ? 
				CashItemType::CashItemType_None : CashItemType::CashItemType_HairStyleCoupon;
		case 514:
			return CashItemType::CashItemType_PersonalStore;
		case 518:
			return CashItemType::CashItemType_WaterOfLife;
		case 513:
			return CashItemType::CashItemType_Charm;
		case 523:
			return CashItemType::CashItemType_ShopScanner;
		case 533:
			return CashItemType::CashItemType_DeliveryTicket;
		case 537:
			return CashItemType::CashItemType_ADBoard;
		case 522:
			return CashItemType::CashItemType_GachaponTicket;
		case 524:
			return CashItemType::CashItemType_PetFood;
		case 525:
			return CashItemType::CashItemType_WeddingTicket;
		case 530:
			return CashItemType::CashItemType_Morph;
		case 538:
			return CashItemType::CashItemType_EvolutionRock;
		case 539:
			return CashItemType::CashItemType_AvatarMegaphone;
		case 540:
			if (nItemID / 1000 == 5400)
				return CashItemType::CashItemType_NameChange;
			if (nItemID / 1000 == 5401)
				return CashItemType::CashItemType_TransferWorld;
			return CashItemType::CashItemType_None;
		case 542:
			if (nItemID / 1000 == 5420)
				return CashItemType::CashItemType_MembershipCoupon;
			return CashItemType::CashItemType_None;
		default:
			return CashItemType::CashItemType_None;
	}
}

int ItemInfo::GetConsumeCashItemType(int nItemID)
{
	int nResult = GetCashSlotItemType(nItemID);
	if (nResult >= CashItemType::CashItemType_MapleTV) 
	{
		if (nResult <= CashItemType::CashItemType_MegaLoveTV)
			return nResult;
		if (nResult == CashItemType::CashItemType_NameChange
			|| nResult == CashItemType::CashItemType_TransferWorld)
			return nResult;
		return 0;
	}

	return nResult;
}

#ifdef _WVSGAME
int ItemInfo::GetWeaponMastery(GA_Character *pCharacter, int nWeaponID, int nSkillID, int nAttackType, int *pnACCInc, int *pnPADInc)
{
	int nWT = GetWeaponType(nWeaponID);
	int nResult1 = 0, nResult2 = 0;
	switch (nWT)
	{
		case 49:
			if (nAttackType != 1 && (nSkillID != 5221003 || nAttackType))
				return 0;
			return SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 5200000, nullptr, pnACCInc);;
		case 48:
			if (nAttackType && (nSkillID != 5121002 || nAttackType != 1))
				return 0;
			return SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 5100001, nullptr, pnACCInc);
		case 47:
			if (nAttackType != 1)
				return 0;
			return SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 4100000, nullptr, pnACCInc);
		case 46:
			if (nAttackType != 1)
				return 0;
			nResult1 = SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 3200000, nullptr, pnACCInc);
			nResult2 = SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 3220004, nullptr, pnPADInc);
			return nResult2 ? nResult2 : nResult1;
		case 45:
			if (nAttackType != 1)
				return 0;
			nResult1 = SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 3100000, nullptr, pnACCInc);
			nResult2 = SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 3120005, nullptr, pnPADInc);
			return nResult2 ? nResult2 : nResult1;
		case 44:
			if (nAttackType)
				return 0;
			return SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 1300001, nullptr, pnACCInc)
				+ SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 1321007, nullptr, pnPADInc);
		case 43:
			if (nAttackType)
				return 0;
			return SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 1300000, nullptr, pnACCInc)
				+ SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 1321007, nullptr, pnPADInc);
		case 42:
		case 32:
			if (nAttackType)
				return 0;
			return SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 1200001, nullptr, pnACCInc);
		case 31:
		case 41:
			if (nAttackType)
				return 0;
			return SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 1100001, nullptr, pnACCInc);
		case 30:
		case 40:
			if (nAttackType)
				return 0;
			nResult1 = SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 1100000, nullptr, pnACCInc);
			nResult2 = SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 1200000, nullptr, pnPADInc);
			return nResult2 ? nResult2 : nResult1;
		default:
			if (nWT > 42 && nAttackType != 1)
				return 0;
			else if (nWT <= 42 && nAttackType)
				return 0;
	}
	return SkillInfo::GetInstance()->GetMasteryFromSkill(pCharacter, 4200000, nullptr, pnACCInc);
}

int ItemInfo::GetWeaponType(int nItemID)
{
	int result = 0;

	if (nItemID / 1000000 != 1
		|| (result = nItemID / 10000 % 100, result < 30)
		|| result > 33 && (result <= 36 || result > 49))
	{
		result = 0;
	}
	return result;
}

int ItemInfo::GetCriticalSkillLevel(GA_Character * pCharacter, int nWeaponID, int nAttackType, int * pnProp, int * pnParam)
{
	SkillEntry* pEntry = nullptr;
	pnParam ? *pnParam = 0 : void();
	pnProp ? *pnProp = 0 : void();
	int nCSLV = 0;
	int nWT = GetWeaponType(nWeaponID);
	if (nWT >= 45)
	{
		if (nAttackType != 1)
			return 0;
		if (nWT == 45 || nWT == 46)
			nCSLV = SkillInfo::GetInstance()->GetSkillLevel(pCharacter, 3000001, &pEntry);
		else if(nWT == 47)
			nCSLV = SkillInfo::GetInstance()->GetSkillLevel(pCharacter, 4100001, &pEntry);
		if (pEntry && nCSLV > 0)
		{
			if (pnProp)
				*pnProp = pEntry->GetLevelData(nCSLV)->m_nProp;
			if (pnParam)
				*pnParam = pEntry->GetLevelData(nCSLV)->m_nDamage;
		}
	}
	return nCSLV;
}
#endif

void ItemInfo::LoadIncrementStat(BasicIncrementStat & refStat, void * pProp)
{
	auto& infoImg = (*((WzIterator*)pProp));
	refStat.niSTR = infoImg["incSTR"];
	refStat.niDEX = infoImg["incDEX"];
	refStat.niINT = infoImg["incINT"];
	refStat.niLUK = infoImg["incLUK"];
	refStat.niMaxHP = infoImg["incMHP"];
	refStat.niMaxMP = infoImg["incMMP"];
	refStat.niPAD = infoImg["incPAD"];
	refStat.niMAD = infoImg["incMAD"];
	refStat.niPDD = infoImg["incPDD"];
	refStat.niMDD = infoImg["incMDD"];
	refStat.niACC = infoImg["incACC"];
	refStat.niEVA = infoImg["incEVA"];
	refStat.niCraft = infoImg["incCraft"];
	refStat.niSpeed = infoImg["incSpeed"];
	refStat.niJump = infoImg["incJump"];
	refStat.niSwim = infoImg["incSwim"];
}

void ItemInfo::LoadAbilityStat(BasicAbilityStat & refStat, void * pProp)
{
	auto& infoImg = (*((WzIterator*)pProp));	
	refStat.nAttribute = atoi(((std::string)infoImg["bagType"]).c_str());
	refStat.nAttribute |= atoi(((std::string)infoImg["notSale"]).c_str()) * ItemAttribute::eNotSale;
	refStat.nAttribute |= atoi(((std::string)infoImg["expireOnLogout"]).c_str()) * ItemAttribute::eExpireOnLogout;
	refStat.nAttribute |= atoi(((std::string)infoImg["pickUpBlock"]).c_str()) * ItemAttribute::ePickUpBlock;
	refStat.nAttribute |= atoi(((std::string)infoImg["equipTradeBlock"]).c_str()) * ItemAttribute::eTradeBlockAfterEquip;
	refStat.nAttribute |= atoi(((std::string)infoImg["only"]).c_str()) * ItemAttribute::eOnly;
	refStat.nAttribute |= atoi(((std::string)infoImg["accountSharable"]).c_str()) * ItemAttribute::eAccountSharable;
	refStat.nAttribute |= atoi(((std::string)infoImg["quest"]).c_str()) * ItemAttribute::eQuest;
	refStat.nAttribute |= atoi(((std::string)infoImg["tradeBlock"]).c_str()) * ItemAttribute::eTradeBlock;
	refStat.nAttribute |= atoi(((std::string)infoImg["accountShareTag"]).c_str()) * ItemAttribute::eAccountShareTag;
	
	int nMobHP = atoi(((std::string)infoImg["mobHP"]).c_str());
	if(nMobHP && nMobHP < 0100)
		refStat.nAttribute |= ItemAttribute::eMobHP;

	refStat.bCash = (atoi(((std::string)infoImg["cash"]).c_str())) == 1;
	refStat.bTimeLimited = (atoi(((std::string)infoImg["timeLimited"]).c_str())) == 1;
}

GW_ItemSlotBase * ItemInfo::GetItemSlot(int nItemID, ItemVariationOption enOption)
{
	int nType = GetItemSlotType(nItemID);
	GW_ItemSlotBase *ret = nullptr;
	if (nType == 1)
	{
		auto pItem = GetEquipItem(nItemID);
		if (pItem == nullptr)
			return nullptr;
		GW_ItemSlotEquip* pEquip = AllocObj(GW_ItemSlotEquip);
		int nValue = 0;
		if ((nValue = pItem->incStat.niSTR))
			pEquip->nSTR = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niDEX))
			pEquip->nDEX = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niLUK))
			pEquip->nLUK = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niINT))
			pEquip->nINT = GetVariation(nValue, enOption);

		if ((nValue = pItem->incStat.niMaxHP))
			pEquip->nMaxHP = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niMaxMP))
			pEquip->nMaxMP = GetVariation(nValue, enOption);

		if ((nValue = pItem->incStat.niPAD))
			pEquip->nPAD = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niPDD))
			pEquip->nPDD = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niMAD))
			pEquip->nMAD = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niMDD))
			pEquip->nMDD = GetVariation(nValue, enOption);

		if ((nValue = pItem->incStat.niACC))
			pEquip->nACC = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niEVA))
			pEquip->nEVA = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niCraft))
			pEquip->nCraft = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niSpeed))
			pEquip->nSpeed = GetVariation(nValue, enOption);
		if ((nValue = pItem->incStat.niJump))
			pEquip->nJump = GetVariation(nValue, enOption);
		pEquip->nRUC = pItem->nRUC;
		pEquip->nCuttable = (pItem->nCuttable == 0 ? 0xFF : pItem->nCuttable);

		ret = pEquip;
		ret->nAttribute = pItem->abilityStat.nAttribute;
		ret->bIsCash = pItem->abilityStat.bCash;
	}
	else if (nType > 1 && nType <= 5)
	{
		auto pItem = GetBundleItem(nItemID);
		if (pItem == nullptr)
		{
			auto pCash = GetCashItem(nItemID);
			if (pCash && pCash->bIsPet)
			{
				ret = AllocObj(GW_ItemSlotPet);
				((GW_ItemSlotPet*)ret)->nLevel = 1;
				((GW_ItemSlotPet*)ret)->strPetName = GetItemName(nItemID);
				ret->bIsCash = ret->bIsPet = true;
				ret->liExpireDate = GameDateTime::GetDateExpireFromPeriod(90);
			}
		}
		else
		{
			ret = AllocObj(GW_ItemSlotBundle);
			((GW_ItemSlotBundle*)ret)->nNumber = 1;
			ret->nAttribute = pItem->abilityStat.nAttribute;
			ret->bIsCash = pItem->abilityStat.bCash;
		}
	}
	if (ret)
	{
		ret->nItemID = nItemID;
		ret->nType = (GW_ItemSlotBundle::GW_ItemSlotType)((nType));
	}
	return ret;
}

int ItemInfo::GetVariation(int v, ItemVariationOption enOption)
{
	int result; // eax@1
	int v3; // eax@3
	unsigned int v4; // edi@5
	signed int v5; // eax@6
	int v6; // ecx@8
	signed int v7; // edi@8
	int v8; // edi@11
	int v9; // eax@16
	unsigned int v10; // edi@18
	signed int v11; // eax@19
	char v12; // cl@21
	signed int v13; // eax@21
	int v14; // edi@21
	bool v15; // zf@24
	bool v16; // cf@28
	int enOptiona; // [sp+18h] [bp+Ch]@5

	result = v;
	if (v)
	{
		if (enOption != 4)
		{
			v9 = v / 10 + 1;
			if (v9 >= 5)
				v9 = 5;
			v10 = 1 << (v9 + 2);
			if (v10)
				v11 = (unsigned int)(Rand32::GetInstance()->Random() % v10);
			else
				v11 = (unsigned int)(Rand32::GetInstance()->Random());
			v12 = v11;
			v13 = v11 >> 1;
			v14 = ((v13 >> 3) & 1)
				+ ((v13 >> 2) & 1)
				+ ((v13 >> 1) & 1)
				+ (v13 & 1)
				+ (v12 & 1)
				- 2
				+ ((v13 >> 4) & 1)
				+ ((v13 >> 5) & 1);
			if (v14 <= 0)
				v14 = 0;
			if (enOption == 2)
			{
				v15 = (Rand32::GetInstance()->Random() & 1) == 0;
				result = v;
				if (v15)
					return v - v14;
			}
			else
			{
				if (enOption == 1)
					v16 = Rand32::GetInstance()->Random() % 0xA < 3;
				else
				{
					if (enOption != 3)
						return v;
					v16 = Rand32::GetInstance()->Random() % 0xA < 1;
				}
				result = v;
				if (v16)
					return result;
			}
			result += v14;
			return result;
		}
		v3 = v / 5 + 1;
		if (v3 >= 7)
			v3 = 7;
		v4 = 1 << (v3 + 2);
		enOptiona = v3 + 2;
		if (v4)
			v5 = (int)(Rand32::GetInstance()->Random() % v4);
		else
			v5 = (int)(Rand32::GetInstance()->Random());
		v6 = enOptiona;
		v7 = -2;
		if ((signed int)enOptiona > 0)
		{
			do
			{
				v7 += v5 & 1;
				v5 >>= 1;
				--v6;
			} while (v6);
		}
		if (Rand32::GetInstance()->Random() & 1)
			v8 = v + v7;
		else
			v8 = v - v7;
		if (v8 <= 0)
			v8 = 0;
		result = v8;
	}
	return result;
}

