#include "GA_Character.hpp"
#include "WvsUnified.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "GW_ItemSlotEquip.h"
#include "GW_ItemSlotBundle.h"
#include "GW_ItemSlotPet.h"
#include "GW_CharacterStat.h"
#include "GW_CharacterLevel.h"
#include "GW_CharacterMoney.h"
#include "GW_CharacterSlotCount.h"
#include "GW_Avatar.hpp"
#include "GW_SkillRecord.h"
#include "GW_QuestRecord.h"

#include <algorithm>

#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Common\WvsGameConstants.hpp"

GA_Character::GA_Character()
	: mAvatarData(AllocObj(GW_Avatar)),
	mStat(AllocObj(GW_CharacterStat)),
	mLevel(AllocObj(GW_CharacterLevel)),
	mMoney(AllocObj(GW_CharacterMoney)),
	mSlotCount(AllocObj(GW_CharacterSlotCount))
{
}

GA_Character::~GA_Character()
{
}

void GA_Character::Load(int nCharacterID)
{
	LoadCharacter(nCharacterID);
	mMoney->Load(nCharacterID);
	mSlotCount->Load(nCharacterID);
	LoadItemSlot();
	LoadSkillRecord();
	LoadQuestRecord();
}

void GA_Character::LoadCharacter(int nCharacterID)
{
	mAvatarData->Load(nCharacterID);
	mStat->Load(nCharacterID);
	mLevel->Load(nCharacterID);

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM `Character` Where CharacterID = " << nCharacterID;
	Poco::Data::RecordSet recordSet(queryStatement);
	queryStatement.execute();
	this->nCharacterID = nCharacterID;
	nAccountID = recordSet["AccountID"];
	nWorldID = recordSet["WorldID"];
	strName = recordSet["CharacterName"].toString();
	//nGuildID = recordSet["GuildID"];
	//nPartyID = recordSet["PartyID"];
	nFieldID = recordSet["FieldID"];
	nFriendMax = recordSet["FriendMaxNum"];
	nGradeCode = recordSet["GradeCode"];
	nActiveEffectItemID = recordSet["ActiveEffectItemID"];
	//nGender = recordSet["Gender"];
}

void GA_Character::EncodeAvatar(OutPacket *oPacket)
{
	EncodeStat(oPacket);
	EncodeAvatarLook(oPacket);
}

void GA_Character::EncodeStat(OutPacket *oPacket)
{
	oPacket->Encode4(nCharacterID);
	oPacket->EncodeBuffer(
		(unsigned char*)strName.c_str(),
		(int)strName.size(),
		13 - (int)strName.size());

	oPacket->Encode1(mStat->nGender);
	oPacket->Encode1(mStat->nSkin);
	oPacket->Encode4(mStat->nFace);
	oPacket->Encode4(mStat->nHair);
	oPacket->EncodeBuffer(nullptr, 24);
	oPacket->Encode1(mLevel->nLevel); //LEVEL
	oPacket->Encode2(mStat->nJob);

	oPacket->Encode2(mStat->nStr);
	oPacket->Encode2(mStat->nDex);
	oPacket->Encode2(mStat->nInt);
	oPacket->Encode2(mStat->nLuk);
	oPacket->Encode2(mStat->nHP);
	oPacket->Encode2(mStat->nMaxHP);
	oPacket->Encode2(mStat->nMP);
	oPacket->Encode2(mStat->nMaxMP);

	oPacket->Encode2(mStat->nAP);
	oPacket->Encode2(mStat->aSP[0]);
	//mStat->EncodeExtendSP(oPacket);
	//oPacket->Encode1(0); //SP

	oPacket->Encode4((int)mStat->nExp); //EXP
	oPacket->Encode2(mStat->nFame);
	oPacket->Encode4(0); //Gach EXP
	oPacket->Encode4(nFieldID);
	oPacket->Encode1(0); //Inital Spawn Point

	oPacket->EncodeBuffer(nullptr, 30);
}

void GA_Character::EncodeSkillRecord(OutPacket * oPacket)
{
	oPacket->Encode2((short)mSkillRecord.size());
	for (auto& skill : mSkillRecord)
		skill.second->Encode(oPacket);
}

void GA_Character::EncodeAvatarLook(OutPacket *oPacket)
{
	oPacket->Encode1(mStat->nGender);
	oPacket->Encode1(mAvatarData->nSkin);
	oPacket->Encode4(mAvatarData->nFace);
	oPacket->Encode1(0); //Mega?
	oPacket->Encode4(mAvatarData->nHair);

	for (const auto& eqp : mAvatarData->mEquip)
	{
		oPacket->Encode1(eqp.first * -1);
		oPacket->Encode4(eqp.second);
	}
	oPacket->Encode1((char)0xFF);
	for (const auto& eqp : mAvatarData->mUnseenEquip)
	{
		oPacket->Encode1((eqp.first * -1) - 100);
		oPacket->Encode4(eqp.second);
	}
	oPacket->Encode1((char)0xFF);

	int nCashWeaponID = 0, nWeaponID = 0, nShieldID = 0;
	for (auto pEqup : mAvatarData->mEquip)
		if (pEqup.first == -111)
			nCashWeaponID = pEqup.second;
		else if (pEqup.first == -11)
			nWeaponID = pEqup.second;
		else if (pEqup.first == -10)
			nShieldID = pEqup.second;

	oPacket->Encode4(nCashWeaponID);
	oPacket->Encode4(nShieldID);

	oPacket->Encode8(0);
}

void GA_Character::Save(bool bIsNewCharacter)
{
	if (bIsNewCharacter)
	{
		nCharacterID = (int)IncCharacterID();
		Poco::Data::Statement newRecordStatement(GET_DB_SESSION);
		newRecordStatement << "INSERT INTO `Character` (CharacterID, AccountID, WorldID) VALUES(" << nCharacterID << ", " << nAccountID << ", " << nWorldID << ")";
		newRecordStatement.execute();
		newRecordStatement.reset(GET_DB_SESSION);
		//newRecordStatement << "SELECT CharacterID FROM Characters"
	}
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "UPDATE `Character` Set "
		<< "CharacterName = '" << strName << "', "
		<< "ActiveEffectItemID = " << nActiveEffectItemID << ", "
		<< "FieldID = " << nFieldID << " WHERE CharacterID = " << nCharacterID;

	queryStatement.execute();
	mAvatarData->Save(nCharacterID, bIsNewCharacter);
	mMoney->Save(nCharacterID, bIsNewCharacter);
	mLevel->Save(nCharacterID, bIsNewCharacter);
	mStat->Save(nCharacterID, bIsNewCharacter);
	mSlotCount->Save(nCharacterID, bIsNewCharacter);

	for (auto& eqp : mItemSlot[1])
		((eqp.second))->Save(nCharacterID);
	for (auto& con : mItemSlot[2])
		((con.second))->Save(nCharacterID);
	for (auto& etc : mItemSlot[3])
		((etc.second))->Save(nCharacterID);
	for (auto& ins : mItemSlot[4])
		((ins.second))->Save(nCharacterID);
	for (auto& ins : mItemSlot[5])
		((ins.second))->Save(nCharacterID);
	SaveInventoryRemovedRecord();

	for (auto& skill : mSkillRecord)
		skill.second->Save();
	for (auto& questRecord : mQuestRecord)
		questRecord.second->Save();
	for (auto& questRecord : mQuestComplete)
		questRecord.second->Save();
}

void GA_Character::SaveInventoryRemovedRecord()
{
	GW_ItemSlotEquip equipRemovedInstance;
	GW_ItemSlotBundle bundleRemovedInstance;
	equipRemovedInstance.nCharacterID = nCharacterID;
	bundleRemovedInstance.nCharacterID = nCharacterID;
	for (int i = 1; i <= 5; ++i)
	{
		for (const auto& liSN : mItemRemovedRecord[i])
			if (i == 1)
			{
				equipRemovedInstance.liItemSN = liSN * -1;
				equipRemovedInstance.nType = GW_ItemSlotBase::GW_ItemSlotType::EQUIP;
				equipRemovedInstance.Save(nCharacterID, true);
			}
			else
			{
				bundleRemovedInstance.liItemSN = liSN * -1;
				bundleRemovedInstance.nType = (GW_ItemSlotBase::GW_ItemSlotType)(i);
				bundleRemovedInstance.Save(nCharacterID, true);
			}
	}
}

int GA_Character::FindEmptySlotPosition(int nTI)
{
	if (nTI <= 0 || nTI > 5)
		return 0;
	int nLastIndex = 1;
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	auto itemSlot = mItemSlot[nTI];
	for (auto& slot : itemSlot)
	{
		if (slot.first < 0) //skip equipped
			continue;
		if (slot.first > mSlotCount->aSlotCount[nTI])
			return 0;
		if (slot.first > nLastIndex || (slot.first == nLastIndex && slot.second == nullptr))
			return nLastIndex;
		nLastIndex = slot.first + 1;
	}
	return nLastIndex > mSlotCount->aSlotCount[nTI] ? 0 : nLastIndex;
}

ZSharedPtr<GW_ItemSlotBase> GA_Character::GetItem(int nTI, int nPOS)
{
	if (nTI <= 0 || nTI > 5)
		return{ nullptr };
	auto result = mItemSlot[nTI].find(nPOS);
	if (result == mItemSlot[nTI].end())
		return{ nullptr };
	return result->second;
}

ZSharedPtr<GW_ItemSlotBase> GA_Character::GetItemByID(int nItemID)
{
	int nTI = nItemID / 1000000;
	if (nTI <= 0 || nTI > 5)
		return{ nullptr };
	auto itemSlot = mItemSlot[nTI];
	for (auto& slot : itemSlot)
	{
		if (slot.first < 0) //skip equipped
			continue;
		if (slot.first > mSlotCount->aSlotCount[nTI])
			return{ nullptr };
		if (slot.second->nItemID == nItemID)
			return slot.second;
	}
	return nullptr;
}

void GA_Character::RemoveItem(int nTI, int nPOS)
{
	if (nTI <= 0 || nTI > 5)
		return;
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	auto pItem = GetItem(nTI, nPOS);
	if (pItem != nullptr)
	{
		mItemSlot[nTI].erase(pItem->nPOS);
		if (pItem->liItemSN != -1)
			mItemRemovedRecord[nTI].insert(pItem->liItemSN);
	}
}

int GA_Character::FindCashItemSlotPosition(int nTI, long long int liSN)
{
	if (nTI <= 0 || nTI > 5)
		return 0;
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	auto& itemSlot = mItemSlot[nTI];
	for (auto& slot : itemSlot)
		if (slot.second->liCashItemSN == liSN)
			return slot.second->nPOS;
	return 0;
}

int GA_Character::FindGeneralItemSlotPosition(int nTI, int nItemID, long long int dateExpire, long long int liSN)
{
	if (nTI <= 0 || nTI > 5)
		return 0;
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	auto& itemSlot = mItemSlot[nTI];
	for (auto& slot : itemSlot)
		if (slot.second->nItemID == nItemID && !CompareFileTime((FILETIME*)&dateExpire, (FILETIME*)(slot.second->liExpireDate)))
			return slot.second->nPOS;
	return 0;
}

int GA_Character::GetEmptySlotCount(int nTI)
{
	int nCount = 0;
	if (nTI <= 0 || nTI > 5)
		return 0;
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	auto& itemSlot = mItemSlot[nTI];
	int nLastIndeex = 0;
	for (auto& slot : itemSlot)
	{
		if (!slot.second)
			++nCount;
		nCount += (slot.first - nLastIndeex - 1);
		nLastIndeex = slot.first;
	}
	return nCount;
}

int GA_Character::GetItemCount(int nTI, int nItemID)
{
	int nCount = 0;
	if (nTI <= 0 || nTI > 5)
		return 0;
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	auto& itemSlot = mItemSlot[nTI];
	for (auto& slot : itemSlot)
		if (slot.second && slot.second->nItemID == nItemID)
			nCount += (slot.second->nType == GW_ItemSlotBase::EQUIP ? 1 :
			((GW_ItemSlotBundle*)(slot.second))->nNumber);
	return nCount;
}

void GA_Character::SetItem(int nTI, int nPOS, const ZSharedPtr<GW_ItemSlotBase>& pItem)
{
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	if (nTI >= 1 && nTI <= 5)
		mItemSlot[nTI][nPOS] = pItem;
}

bool GA_Character::IsWearing(int nEquipItemID)
{
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	auto& itemSlot = mItemSlot[1];
	for (auto& slot : itemSlot)
		if (slot.second
			&& slot.second->nItemID == nEquipItemID
			&& slot.second->nPOS < 0)
			return true;
		else if (slot.second->nPOS >= 0)
			return false;
	return false;
}

decltype(GA_Character::mSkillRecord)& GA_Character::GetCharacterSkillRecord()
{
	return mSkillRecord;
}

GW_SkillRecord * GA_Character::GetSkill(int nSkillID)
{
	auto findResult = mSkillRecord.find(nSkillID);
	return findResult == mSkillRecord.end() ? nullptr : (GW_SkillRecord *)findResult->second;
}

void GA_Character::ObtainSkillRecord(GW_SkillRecord * pRecord)
{
	std::lock_guard<std::mutex> dataLock(mCharacterLock);
	if (pRecord != nullptr)
		mSkillRecord.insert({ pRecord->nSkillID, pRecord });
}

void GA_Character::SetQuest(int nKey, const std::string & sInfo)
{
	auto findIter = mQuestRecord.find(nKey);
	if (findIter == mQuestRecord.end())
	{
		GW_QuestRecord *pNewRecord = AllocObj(GW_QuestRecord);
		pNewRecord->nState = 1;
		pNewRecord->nQuestID = nKey;
		pNewRecord->sStringRecord = sInfo;
		mQuestRecord.insert({ nKey, pNewRecord });
	}
	else
		findIter->second->sStringRecord = sInfo;
}

void GA_Character::RemoveQuest(int nKey)
{
	mQuestRecord.erase(nKey);
}

bool GA_Character::SetTrading(bool bTrade)
{
	if (bOnTrading == bTrade)
		return false;
	bOnTrading = bTrade;
	ClearTradeInfo();
}

void GA_Character::ClearTradeInfo()
{
	nMoneyTrading = 0;
	for (int i = 1; i <= 5; ++i)
		mItemTrading[i].clear();
}

int GA_Character::GetTradingCount(int nTI, int nPOS)
{
	auto findIter = mItemTrading[nTI].find(nPOS);
	return findIter == mItemTrading[nTI].end() ? 0 : findIter->second;
}

std::mutex & GA_Character::GetCharacterDataLock()
{
	return mCharacterLock;
}

void GA_Character::DecodeCharacterData(InPacket *iPacket, bool bForInternal)
{
	long long int flag = iPacket->Decode8();

	if (flag & 1)
	{
		DecodeStat(iPacket);
		nFriendMax = iPacket->Decode1(); //BUDDY LIST CAPACITY
		iPacket->Decode8(); //Current Time
	}

	if (flag & 2)
	{
		mMoney->nMoney = iPacket->Decode4();
		nCharacterID = iPacket->Decode4();
		iPacket->Decode4();
		iPacket->Decode4();
	}
	for (int i = 1; i <= 5; ++i)
		mSlotCount->aSlotCount[i] = iPacket->Decode1();

	DecodeItemSlot(iPacket, bForInternal);
	if (flag & 0x100)
		DecodeSkillRecord(iPacket);

	if (flag & 0x8000)
	{
		int nCoolDownSize = iPacket->Decode2();
		for (int i = 0; i < nCoolDownSize; ++i)
		{

		}
	}

	if (flag & 0x200)
	{
		int nStartedQuestSize = iPacket->Decode2();
		for (int i = 0; i < nStartedQuestSize; ++i)
		{
			GW_QuestRecord *pRecord = AllocObj(GW_QuestRecord);
			pRecord->nCharacterID = nCharacterID;
			pRecord->nState = 1;
			pRecord->Decode(iPacket, 1);
			mQuestRecord.insert({ pRecord->nQuestID, pRecord });
		}
	}

	if (flag & 0x4000)
	{
		int nCompletedQuestSize = iPacket->Decode2();
		for (int i = 0; i < nCompletedQuestSize; ++i)
		{
			GW_QuestRecord *pRecord = AllocObj(GW_QuestRecord);
			pRecord->nCharacterID = nCharacterID;
			pRecord->nState = 2;
			pRecord->Decode(iPacket, 2);
			mQuestComplete.insert({ pRecord->nQuestID, pRecord });
		}
	}

	if (flag & 0x400)
	{
		iPacket->Decode2();
	}

	if (flag & 0x800)
	{
		int nCoupleRingSize = iPacket->Decode2();
		int nFriendRingSize = iPacket->Decode2();
		int nMarriageRingSize = iPacket->Decode2();
	}

	if (flag & 0x1000) //ROCK INFO
	{
		for (int i = 0; i < 5; ++i)
			iPacket->Decode4();
		for (int i = 0; i < 10; ++i)
			iPacket->Decode4();
	}

	//ALWAYS PUT AT DOWNMOST PLACE.
	if (bForInternal)
		DecodeInternalData(iPacket);
}

void GA_Character::DecodeInternalData(InPacket * iPacket)
{
	nGradeCode = iPacket->Decode1();
	nActiveEffectItemID = iPacket->Decode4();
}

void GA_Character::DecodeStat(InPacket *iPacket)
{
	nCharacterID = iPacket->Decode4();

	char cStrNameBuff[14]{ 0 };
	iPacket->DecodeBuffer((unsigned char*)cStrNameBuff, 13);
	strName = std::string(cStrNameBuff);

	mStat->nGender = iPacket->Decode1();
	mStat->nSkin = iPacket->Decode1();
	mStat->nFace = iPacket->Decode4();
	mStat->nHair = iPacket->Decode4();
	iPacket->DecodeBuffer(nullptr, 24);

	mLevel->nLevel = iPacket->Decode1(); //LEVEL
	mStat->nJob = iPacket->Decode2();

	mStat->nStr = iPacket->Decode2();
	mStat->nDex = iPacket->Decode2();
	mStat->nInt = iPacket->Decode2();
	mStat->nLuk = iPacket->Decode2();
	mStat->nHP = iPacket->Decode2();
	mStat->nMaxHP = iPacket->Decode2();
	mStat->nMP = iPacket->Decode2();
	mStat->nMaxMP = iPacket->Decode2();

	mStat->nAP = iPacket->Decode2();
	mStat->aSP[0] = iPacket->Decode2();
	//iPacket->Decode1(); //SP

	mStat->nExp = iPacket->Decode4(); //EXP
	mStat->nFame = iPacket->Decode2();
	iPacket->Decode4(); //nGachaponEXP
	nFieldID = iPacket->Decode4();
	iPacket->Decode1(); //Inital Spawn Point

	iPacket->DecodeBuffer(nullptr, 30);

}

void GA_Character::DecodeItemSlot(InPacket *iPacket, bool bForInternal)
{
	//bForInternal = true 代表Center與Game, Shop之前的傳遞，作用在於標記哪些物品已刪除，便於存檔時直接修改掉CharacterID
	if (bForInternal)
		DecodeInventoryRemovedRecord(iPacket);

	unsigned char nPos = 0;

	while ((nPos = iPacket->Decode1()) != 0)
	{
		GW_ItemSlotEquip* eqp = AllocObj(GW_ItemSlotEquip);
		eqp->nPOS = nPos * -1;
		eqp->nType = GW_ItemSlotBase::GW_ItemSlotType::EQUIP;
		eqp->Decode(iPacket, bForInternal);
		mItemSlot[1].insert({ eqp->nPOS , eqp });
	}

	while ((nPos = iPacket->Decode1()) != 0)
	{
		GW_ItemSlotEquip* eqp = AllocObj(GW_ItemSlotEquip);
		eqp->nPOS = (nPos + 100) * -1;
		eqp->nType = GW_ItemSlotBase::GW_ItemSlotType::EQUIP;
		eqp->Decode(iPacket, bForInternal);
		mItemSlot[1].insert({ eqp->nPOS, eqp });
	}

	while ((nPos = iPacket->Decode1()) != 0)
	{
		GW_ItemSlotEquip* eqp = AllocObj(GW_ItemSlotEquip);
		eqp->nPOS = nPos;
		eqp->nType = GW_ItemSlotBase::GW_ItemSlotType::EQUIP;
		eqp->Decode(iPacket, bForInternal);
		mItemSlot[1].insert({ eqp->nPOS, eqp });
	}

	while ((nPos = iPacket->Decode1()) != 0)
	{
		GW_ItemSlotBundle* bundle = AllocObj(GW_ItemSlotBundle);
		bundle->nPOS = nPos;
		bundle->nType = GW_ItemSlotBase::GW_ItemSlotType::CONSUME;
		bundle->Decode(iPacket, bForInternal);
		mItemSlot[2].insert({ bundle->nPOS, bundle });
	}

	while ((nPos = iPacket->Decode1()) != 0)
	{
		GW_ItemSlotBundle* bundle = AllocObj(GW_ItemSlotBundle);
		bundle->nPOS = nPos;
		bundle->nType = GW_ItemSlotBase::GW_ItemSlotType::INSTALL;
		bundle->Decode(iPacket, bForInternal);
		mItemSlot[3].insert({ bundle->nPOS, bundle });
	}

	while ((nPos = iPacket->Decode1()) != 0)
	{
		GW_ItemSlotBundle* bundle = AllocObj(GW_ItemSlotBundle);
		bundle->nPOS = nPos;
		bundle->nType = GW_ItemSlotBase::GW_ItemSlotType::ETC;
		bundle->Decode(iPacket, bForInternal);
		mItemSlot[4].insert({ bundle->nPOS, bundle });
	}

	while ((nPos = iPacket->Decode1()) != 0)
	{
		GW_ItemSlotBase* pCash = nullptr;
		int nType = iPacket->Decode1();
		iPacket->Offset(-1);
		if (nType == 3)
			pCash = AllocObj(GW_ItemSlotPet);
		else
			pCash = AllocObj(GW_ItemSlotBundle);

		pCash->nPOS = nPos;
		pCash->nType = GW_ItemSlotBase::GW_ItemSlotType::CASH;
		pCash->Decode(iPacket, bForInternal);
		mItemSlot[5].insert({ pCash->nPOS, pCash });
	}
}

void GA_Character::DecodeInventoryRemovedRecord(InPacket * iPacket)
{
	long long int liItemSN_ = -1;
	for (int i = 1; i <= 4; ++i)
		while ((liItemSN_ = iPacket->Decode8()), liItemSN_ != -1)
			mItemRemovedRecord[i].insert(liItemSN_);
}

void GA_Character::DecodeAvatarLook(InPacket * iPacket)
{
	mAvatarData->Decode(iPacket);
}

void GA_Character::DecodeSkillRecord(InPacket * iPacket)
{
	short nCount = iPacket->Decode2();
	for (int i = 0; i < nCount; ++i)
	{
		GW_SkillRecord* pSkillRecord = AllocObj(GW_SkillRecord);
		pSkillRecord->nMasterLevel = 0;
		pSkillRecord->Decode(iPacket);
		pSkillRecord->nCharacterID = nCharacterID;
		mSkillRecord.insert({ pSkillRecord->nSkillID, pSkillRecord });
	}
}

void GA_Character::EncodeCharacterData(OutPacket *oPacket, bool bForInternal)
{
	long long int flag = 0xFFFFFFFFFFFFFFFF;
	oPacket->Encode8(flag);

	if (flag & 1)
	{
		EncodeStat(oPacket);
		oPacket->Encode1((char)nFriendMax); //BUDDY LIST CAPACITY
		oPacket->Encode8(GameDateTime::GetCurrentDate()); //TIME CURRENT TIME
	}

	if (flag & 2)
	{
		oPacket->Encode4((int)mMoney->nMoney);
		oPacket->Encode4(nCharacterID);
		oPacket->Encode4(100);
		oPacket->Encode4(20);
	}

	for (int i = 1; i <= 5; ++i)
		oPacket->Encode1(mSlotCount->aSlotCount[i]);

	EncodeItemSlot(oPacket, bForInternal);

	if (flag & 0x100)
	{
		EncodeSkillRecord(oPacket);
	}

	if (flag & 0x8000)
	{
		oPacket->Encode2(0);
	}

	if (flag & 0x200)
	{
		oPacket->Encode2((short)mQuestRecord.size());
		for (auto& record : mQuestRecord)
			record.second->Encode(oPacket);
	}

	if (flag & 0x4000)
	{
		oPacket->Encode2((short)mQuestComplete.size());
		for (auto& record : mQuestComplete)
			record.second->Encode(oPacket);
	}

	if (flag & 0x400)
	{
		oPacket->Encode2(0);
	}

	if (flag & 0x800)
	{
		oPacket->Encode2(0);
		oPacket->Encode2(0);
		oPacket->Encode2(0);
	}

	if (flag & 0x1000) //ROCK INFO
	{
		for (int i = 0; i < 5; ++i)
			oPacket->Encode4(0);
		for (int i = 0; i < 10; ++i)
			oPacket->Encode4(0);
	}

	//ALWAYS PUT AT DOWNMOST PLACE.
	if (bForInternal)
		EncodeInternalData(oPacket);
}

void GA_Character::EncodeInternalData(OutPacket *oPacket)
{
	oPacket->Encode1(nGradeCode);
	oPacket->Encode4(nActiveEffectItemID);
}

void GA_Character::EncodeItemSlot(OutPacket *oPacket, bool bForInternal)
{
	//bForInternal = true 代表Center與Game, Shop之前的傳遞，作用在於標記哪些物品已刪除，便於存檔時直接修改掉CharacterID
	if (bForInternal)
		EncodeInventoryRemovedRecord(oPacket);

	for (const auto &eqp : mItemSlot[1])
		if (eqp.second->nPOS < 0 && eqp.second->nPOS > -100)
			eqp.second->Encode(oPacket, bForInternal);
	oPacket->Encode1(0); //EQUIPPED

	for (const auto &eqp : mItemSlot[1])
		if (eqp.second->nPOS <= -100 && eqp.second->nPOS > -1000)
			eqp.second->Encode(oPacket, bForInternal);
	oPacket->Encode1(0); //EQUIPPED 2

	for (const auto &eqp : mItemSlot[1])
		if (eqp.second->nPOS >= 0) //Equip
			eqp.second->Encode(oPacket, bForInternal);
	oPacket->Encode1(0);

	for (auto &item : mItemSlot[2])
		item.second->Encode(oPacket, bForInternal);
	oPacket->Encode1(0); //USE

	for (auto &item : mItemSlot[3])
		item.second->Encode(oPacket, bForInternal);
	oPacket->Encode1(0); //INS

	for (auto &item : mItemSlot[4])
		item.second->Encode(oPacket, bForInternal);
	oPacket->Encode1(0); //ETC

	for (auto &item : mItemSlot[5])
		item.second->Encode(oPacket, bForInternal);
	oPacket->Encode1(0); //CASH
}

void GA_Character::EncodeInventoryRemovedRecord(OutPacket * oPacket)
{
	for (int i = 1; i <= 4; ++i)
	{
		for (const auto& liSN : mItemRemovedRecord[i])
			oPacket->Encode8(liSN);
		oPacket->Encode8(-1);
	}
}

GA_Character::ATOMIC_COUNT_TYPE GA_Character::InitCharacterID()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT MAX(CharacterID) From `Character`";
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	if (recordSet.rowCount() == 0)
		return 0;
	return (ATOMIC_COUNT_TYPE)recordSet["MAX(CharacterID)"];
}

void GA_Character::LoadItemSlot()
{
	GW_ItemSlotBase::LoadAll(GW_ItemSlotBase::EQUIP, nCharacterID, false, false, mItemSlot[1]);
	GW_ItemSlotBase::LoadAll(GW_ItemSlotBase::EQUIP, nCharacterID, true, false, mItemSlot[1]);
	GW_ItemSlotBase::LoadAll(GW_ItemSlotBase::CONSUME, nCharacterID, false, false, mItemSlot[2]);
	GW_ItemSlotBase::LoadAll(GW_ItemSlotBase::INSTALL, nCharacterID, false, false, mItemSlot[3]);
	GW_ItemSlotBase::LoadAll(GW_ItemSlotBase::ETC, nCharacterID, false, false, mItemSlot[4]);
	GW_ItemSlotBase::LoadAll(GW_ItemSlotBase::CASH, nCharacterID, true, false, mItemSlot[5]);
	GW_ItemSlotBase::LoadAll(GW_ItemSlotBase::CASH, nCharacterID, true, true, mItemSlot[5]);
}

void GA_Character::LoadSkillRecord()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM SkillRecord Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);

	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		GW_SkillRecord* pSkillRecord = AllocObj(GW_SkillRecord);
		pSkillRecord->Load((void*)&recordSet);
		mSkillRecord.insert({ pSkillRecord->nSkillID, pSkillRecord });
	}
}

void GA_Character::LoadQuestRecord()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM QuestRecord Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);

	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		GW_QuestRecord* pQuestRecord = AllocObj(GW_QuestRecord);
		pQuestRecord->Load((void*)&recordSet);
		if (pQuestRecord->nState == 1)
			mQuestRecord.insert({ pQuestRecord->nQuestID, pQuestRecord });
		else
			mQuestComplete.insert({ pQuestRecord->nQuestID, pQuestRecord });
	}
}
