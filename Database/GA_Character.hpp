#pragma once
#include <atomic>
#include <mutex>
#include <set>
#include <map>
#include "..\WvsLib\Memory\ZMemory.h"

struct GW_ItemSlotBase;
struct GW_ItemSlotEquip;
struct GW_ItemSlotBundle;
struct GW_CharacterStat;
struct GW_CharacterLevel;
struct GW_CharacterMoney;
struct GW_CharacterSlotCount;
struct GW_SkillRecord;
struct GW_QuestRecord;
struct GW_Avatar;

class InPacket;
class OutPacket;

struct GA_Character
{
private:
	typedef long long int ATOMIC_COUNT_TYPE;
	static ATOMIC_COUNT_TYPE InitCharacterID();

	static ATOMIC_COUNT_TYPE IncCharacterID()
	{
		static std::atomic<ATOMIC_COUNT_TYPE> atomicCharacterID = InitCharacterID();
		return ++atomicCharacterID;
	}

	std::mutex mCharacterLock;

#ifdef DBLIB
	void LoadItemSlot();
	void LoadSkillRecord();
	void LoadQuestRecord();
#endif

public:

	bool bOnTrading = false;
	int nWorldID, 
		nAccountID, 
		nGuildID, 
		nPartyID, 
		nFieldID, 
		nCharacterID, 
		nFriendMax, 
		nMoneyTrading = 0, 
		nGradeCode = 0,
		nActiveEffectItemID = 0;

	std::string strName;
	ZUniquePtr<GW_Avatar> mAvatarData = nullptr;
	ZUniquePtr<GW_CharacterStat> mStat = nullptr;
	ZUniquePtr<GW_CharacterLevel> mLevel = nullptr;
	ZUniquePtr<GW_CharacterMoney> mMoney = nullptr;
	ZUniquePtr<GW_CharacterSlotCount> mSlotCount = nullptr;

	std::map<int, ZUniquePtr<GW_SkillRecord>> mSkillRecord;
	std::map<int, ZUniquePtr<GW_QuestRecord>> mQuestRecord;
	std::map<int, ZUniquePtr<GW_QuestRecord>> mQuestComplete;
	std::map<int, ZSharedPtr<GW_ItemSlotBase>> mItemSlot[6];
	std::map<int, int> mItemTrading[6];

	//For recording the liItemSN of the item which was dropped or removed.
	//09/12/2019 change T from long long int to std::pair<long long int, bool> 
	//where the bool value represents "bIsCash".
	std::set<std::pair<long long int, bool>> mItemRemovedRecord[6];

	void Load(int nCharacterID);
	void LoadCharacter(int nCharacterID);
	void Save(bool isNewCharacter = false);
	void SaveInventoryRemovedRecord();

	void DecodeStat(InPacket *iPacket);
	void DecodeCharacterData(InPacket *iPacket, bool bForInternal);
	void DecodeInternalData(InPacket *iPacket);
	void DecodeItemSlot(InPacket *iPacket, bool bForInternal);
	void DecodeInventoryRemovedRecord(InPacket *iPacket);
	void DecodeAvatarLook(InPacket* iPacket);
	void DecodeSkillRecord(InPacket* iPacket);

	void EncodeCharacterData(OutPacket *oPacket, bool bForInternal);
	void EncodeInternalData(OutPacket *oPacket);
	void EncodeItemSlot(OutPacket *oPacket, bool bForInternal);
	void EncodeInventoryRemovedRecord(OutPacket *oPacket);
	void EncodeAvatar(OutPacket *oPacket);
	void EncodeAvatarLook(OutPacket *oPacket);
	void EncodeStat(OutPacket *oPacket);
	void EncodeSkillRecord(OutPacket *oPacket);

	GA_Character();
	~GA_Character();

	//GW_ItemSlot
	ZSharedPtr<GW_ItemSlotBase> GetItem(int nTI, int nPOS);
	ZSharedPtr<GW_ItemSlotBase> GetItemByID(int nItemID);
	int FindEmptySlotPosition(int nTI);
	void RemoveItem(int nTI, int nPOS);
	int FindCashItemSlotPosition(int nTI, long long int liSN);
	int FindGeneralItemSlotPosition(int nTI, int nItemID, long long int dateExpire, long long int liSN);
	int GetEmptySlotCount(int nTI);
	int GetItemCount(int nTI, int nItemID);
	void SetItem(int nTI, int nPOS, const ZSharedPtr<GW_ItemSlotBase>& pItem);
	bool IsWearing(int nEquipItemID);

	//GW_SkillRecord
	decltype(mSkillRecord)& GetCharacterSkillRecord();
	GW_SkillRecord* GetSkill(int nSkillID);
	void ObtainSkillRecord(GW_SkillRecord* pRecord);

	//QuestRecord
	void SetQuest(int nKey, const std::string& sInfo);
	void RemoveQuest(int nKey);

	//Trading
	bool SetTrading(bool bTrade);
	void ClearTradeInfo();
	int GetTradingCount(int nTI, int nPOS);

	std::mutex& GetCharacterDataLock();
};