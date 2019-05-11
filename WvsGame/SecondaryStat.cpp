#include "SecondaryStat.h"
#include "BasicStat.h"
#include "ItemInfo.h"
#include "SkillInfo.h"
#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "USkill.h"
#include "User.h"
#include "Field.h"
#include "LifePool.h"

#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GW_CharacterLevel.h"

#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"

#include "..\WvsLib\Logger\WvsLogger.h"

#define CHECK_TS_NORMAL(name) \
if (flag & GET_TS_FLAG(name)) \
{ if (EnDecode4Byte(flag))oPacket->Encode4(n##name); else oPacket->Encode2(n##name);\
oPacket->Encode4(r##name); oPacket->Encode4(t##name); }


SecondaryStat::SecondaryStat()
{
}


SecondaryStat::~SecondaryStat()
{
}

void SecondaryStat::SetFrom(GA_Character * pChar, BasicStat * pBS)
{
	const GW_CharacterStat *pCS = pChar->mStat;

	this->nPAD = 0;
	this->nPDD = 0;

	//不知道新的計算公式為何
	this->nMAD = pBS->nINT;
	this->nMDD = pBS->nINT;

	this->nEVA = pBS->nLUK / 2 + pBS->nDEX / 4;
	this->nACC = pBS->nDEX + pBS->nLUK;
	this->nSpeed = 100;
	this->nJump = 100;
	this->nCraft = pBS->nDEX + pBS->nLUK + pBS->nINT;

	int nPDDIncRate = 0; //shield mastery ?

	const GW_ItemSlotEquip* pEquip;
	for (const auto& itemEquipped : pChar->mItemSlot[1])
	{
		pEquip = (const GW_ItemSlotEquip*)itemEquipped.second;

		nPDD += pEquip->nPDD;
		nPAD += pEquip->nPAD;
		nMDD += pEquip->nMDD;
		nMAD += pEquip->nMAD;

		nACC += pEquip->nACC;
		nEVA += pEquip->nEVA;
	}
	SkillEntry
		*pShoutOfEmpress = nullptr,
		*pMichaelShoutOfEmpress = nullptr,
		*pUltimateAdventurer = nullptr,
		*pReinforcementOfEmpress = nullptr;

	int nCheckSLV = 0;
	nCheckSLV = SkillInfo::GetInstance()->GetSkillLevel(pChar, 10000074, &pShoutOfEmpress, 0, 0, 0, 1);
	if (nCheckSLV && pShoutOfEmpress)
	{
		auto pLevelData = pShoutOfEmpress->GetLevelData(nCheckSLV);
		nIncMaxHPr = pLevelData->m_nX;
		nIncMaxMPr = pLevelData->m_nX;
	}

	nCheckSLV = SkillInfo::GetInstance()->GetSkillLevel(pChar, 50000074, &pMichaelShoutOfEmpress, 0, 0, 0, 1);
	if (nCheckSLV && pMichaelShoutOfEmpress)
	{
		auto pLevelData = pMichaelShoutOfEmpress->GetLevelData(nCheckSLV);
		nIncMaxHPr = pLevelData->m_nX;
		nIncMaxMPr = pLevelData->m_nX;
	}

	nCheckSLV = SkillInfo::GetInstance()->GetSkillLevel(pChar, 74, &pUltimateAdventurer, 0, 0, 0, 1);
	if (nCheckSLV && pUltimateAdventurer)
	{
		auto pLevelData = pUltimateAdventurer->GetLevelData(nCheckSLV);
		nLevel = pLevelData->m_nX;
	}

	nCheckSLV = SkillInfo::GetInstance()->GetSkillLevel(pChar, 80, &pReinforcementOfEmpress, 0, 0, 0, 1);
	if (nCheckSLV && pReinforcementOfEmpress)
	{
		auto pLevelData = pReinforcementOfEmpress->GetLevelData(nCheckSLV);
		nLevel = pLevelData->m_nX;
	}

	nIncMaxHPr_Forced = nIncMaxHPr;
}

void SecondaryStat::EncodeForLocal(OutPacket * oPacket, TemporaryStat::TS_Flag & flag)
{
	flag.Encode(oPacket);
	CHECK_TS_NORMAL(PAD);
	CHECK_TS_NORMAL(PDD);
	CHECK_TS_NORMAL(MAD);
	CHECK_TS_NORMAL(MDD);
	CHECK_TS_NORMAL(ACC);
	CHECK_TS_NORMAL(EVA);
	CHECK_TS_NORMAL(Craft);
	CHECK_TS_NORMAL(Speed);
	CHECK_TS_NORMAL(Jump);

	int nCount = 0;
	oPacket->Encode2(nCount);
	for (int i = 0; i < nCount; ++i)
	{
		oPacket->Encode4(0); //mBuffedForSpecMap
		oPacket->Encode1(0); //bEnable
	}
	oPacket->Encode1((int)nDefenseAtt);
	oPacket->Encode1((int)nDefenseState);
	oPacket->Encode1((int)nPVPDamage);


	for (int i = 0; i < 8; ++i)
	{
		if (flag & TemporaryStat::TS_Flag(TemporaryStat::TS_EnergyCharged + i))
		{
			oPacket->Encode4(0);
			oPacket->Encode4(0);
			oPacket->Encode1(0);
			oPacket->Encode4(0);
		}
	}
	//EncodeIndieTempStat(oPacket, flag);
	//EecodeIndieTempStat

	oPacket->Encode2(1);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
	oPacket->Encode4(0);
	oPacket->Encode1(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);

	WvsLogger::LogRaw(WvsLogger::LEVEL_INFO, "Encode Local TS : \n");
	oPacket->Print();
}

void SecondaryStat::EncodeForRemote(OutPacket * oPacket, TemporaryStat::TS_Flag & flag)
{
	flag.Encode(oPacket);

	oPacket->Encode1((char)nDefenseAtt);
	oPacket->Encode1((char)nDefenseState);
	//RideVechicle
}

bool SecondaryStat::EnDecode4Byte(TemporaryStat::TS_Flag & flag)
{
	/*if ((flag & GET_TS_FLAG(CarnivalDefence))
		|| (flag & GET_TS_FLAG(SpiritLink))
		|| (flag & GET_TS_FLAG(DojangLuckyBonus))
		|| (flag & GET_TS_FLAG(SoulGazeCriDamR))
		|| (flag & GET_TS_FLAG(PowerTransferGauge))
		|| (flag & GET_TS_FLAG(ReturnTeleport))
		|| (flag & GET_TS_FLAG(ShadowPartner))
		|| (flag & GET_TS_FLAG(SetBaseDamage))
		|| (flag & GET_TS_FLAG(QuiverCatridge))
		|| (flag & GET_TS_FLAG(ImmuneBarrier))
		|| (flag & GET_TS_FLAG(NaviFlying))
		|| (flag & GET_TS_FLAG(Dance))
		|| (flag & GET_TS_FLAG(SetBaseDamageByBuff))
		|| (flag & GET_TS_FLAG(DotHealHPPerSecond))
		|| (flag & GET_TS_FLAG(MagnetArea))
		|| (flag & GET_TS_FLAG(RideVehicle)))
		return true;*/
	WvsLogger::LogRaw(WvsLogger::LEVEL_INFO, "EnDecode4Byte [False]\n");
	return false;
}

void SecondaryStat::ResetByTime(User* pUser, int tCur)
{
	std::vector<int> aSkillResetReason;
	auto pSS = pUser->GetSecondaryStat();
	for (auto& setFlag : pSS->m_mSetByTS)
	{
		int nID = *(setFlag.second.second[1]);
		int tValue = *(setFlag.second.second[2]);
		if (!((tCur - setFlag.second.first) > tValue))
			continue;
		if (nID < 0)
		{
			auto pItemInfo = ItemInfo::GetInstance()->GetStateChangeItem(-nID);
			if (pItemInfo)
				pUser->SendTemporaryStatReset(pItemInfo->Apply(pUser, 0, false, true));
		}
		else 
			aSkillResetReason.push_back(nID);
	}
	USkill::ResetTemporaryByTime(pUser, aSkillResetReason);
}

void SecondaryStat::DecodeInternal(User* pUser, InPacket * iPacket)
{
	bool bDecodeInternal = iPacket->Decode1() == 1;
	if (!bDecodeInternal)
		return;
	int nChannelID = iPacket->Decode4();

	//Decode Temporary Internal
	int nCount = iPacket->Decode4(), nSkillID, tDurationRemained, nSLV;
	for (int i = 0; i < nCount; ++i)
	{
		nSkillID = iPacket->Decode4();
		tDurationRemained = iPacket->Decode4();
		nSLV = iPacket->Decode4();
		WvsLogger::LogFormat("Decode Internal ID = %d, tValue = %d, nSLV = %d\n", nSkillID, tDurationRemained, nSLV);
		if (nSkillID < 0)
		{
			auto pItem = ItemInfo::GetInstance()->GetStateChangeItem(-nSkillID);
			if (pItem)
				pItem->Apply(pUser, 0, false, false, true, tDurationRemained);
		}
		else
			USkill::OnSkillUseRequest(
				pUser,
				nullptr,
				SkillInfo::GetInstance()->GetSkillByID(nSkillID),
				nSLV,
				false,
				true,
				tDurationRemained
			);
	}
}

void SecondaryStat::EncodeInternal(User* pUser, OutPacket * oPacket)
{
	std::lock_guard<std::recursive_mutex> userGuard(pUser->GetLock());

	oPacket->Encode4(pUser->GetChannelID());

	//Encode Temporary Internal
	auto pSS = pUser->GetSecondaryStat();
	oPacket->Encode4((int)pSS->m_mSetByTS.size());
	for (auto& setFlag : pSS->m_mSetByTS)
	{
		int nSkillID = *(setFlag.second.second[1]);
		int tDurationRemained = (int)setFlag.second.first;
		int nSLV = *(setFlag.second.second[3]);
		oPacket->Encode4(nSkillID);
		oPacket->Encode4(tDurationRemained);
		oPacket->Encode4(nSLV);
	}
}