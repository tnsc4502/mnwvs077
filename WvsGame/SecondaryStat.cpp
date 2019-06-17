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
#include "MobSkillEntry.h"
#include "Summoned.h"
#include "SummonedPool.h"

#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_CharacterLevel.h"

#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"

#include "..\WvsLib\Logger\WvsLogger.h"

#define CHECK_TS_REMOTE_N(name, size)\
if(flag & GET_TS_FLAG(name))\
	oPacket->Encode##size(n##name##_)

#define CHECK_TS_REMOTE_R(name, size)\
if(flag & GET_TS_FLAG(name))\
	oPacket->Encode##size(r##name##_)

#define CHECK_TS_NORMAL(name) \
if (flag & GET_TS_FLAG(name)) { \
oPacket->Encode2(n##name##_);\
oPacket->Encode4(r##name##_); \
oPacket->Encode4(t##name##_); }

decltype(&SecondaryStat::EncodeForLocal) SecondaryStat::ms_aEncoder[] =
{
	&SecondaryStat::EncodeEnergyCharged,
	&SecondaryStat::EncodeDash_Speed,
	&SecondaryStat::EncodeDash_Jump,
	&SecondaryStat::EncodeRideVehicle,
	&SecondaryStat::EncodePartyBooster,
	&SecondaryStat::EncodeGuidedBullet
};

SecondaryStat::SecondaryStat()
{
	m_tsFlagSet = TemporaryStat::TS_Flag::GetDefault();
}


SecondaryStat::~SecondaryStat()
{
}

void SecondaryStat::SetFrom(GA_Character * pChar, BasicStat * pBS)
{
#undef min
#undef max
	const GW_CharacterStat *pCS = pChar->mStat;

	//Set from basic stats
	int nJobType = pCS->nJob / 100;
	this->nPAD = 0;
	this->nPDD = 0;
	this->nMAD = pBS->nINT;
	this->nMDD = pBS->nINT;
	this->nEVA = pBS->nLUK / 2 + pBS->nDEX / 4;
	if(nJobType == 3 || nJobType == 4)
		this->nACC = (int)((double)pBS->nDEX * 0.3 + (double)pBS->nLUK * 0.6);
	else
		this->nACC = (int)((double)pBS->nDEX * 0.8 + (double)pBS->nLUK * 0.5);

	this->nSpeed = 100;
	this->nJump = 100;
	this->nCraft = pBS->nDEX + pBS->nLUK + pBS->nINT;
	int nPDDIncRate = 100; //shield mastery ?

	//Inc from equips
	const GW_ItemSlotEquip* pEquip, *pWeapon = nullptr;
	for (const auto& itemEquipped : pChar->mItemSlot[1])
	{
		pEquip = (const GW_ItemSlotEquip*)itemEquipped.second;
		if (pEquip->nPOS >= 0)
			break;
		if (pEquip->nPOS == -11)
			pWeapon = pEquip;

		if (pEquip->nPOS == -18)
			xRideVehicle_ = pEquip->nItemID;
		if (pEquip->nPOS == -118)
			yRideVehicle_ = pEquip->nItemID;

		nPDD += pEquip->nPDD;
		nPAD += pEquip->nPAD;
		nMDD += pEquip->nMDD;
		nMAD += pEquip->nMAD;

		nACC += pEquip->nACC;
		nEVA += pEquip->nEVA;
		nSpeed += pEquip->nSpeed;
		nJump += pEquip->nJump;
	}

	//Inc from skills
	SkillEntry *pEntry1 = nullptr, *pEntry2 = nullptr;
	int nSLV1 = SkillInfo::GetInstance()->GetSkillLevel(pChar, 3000000, &pEntry1, 0, 0, 0, 0);
	int nSLV2 = SkillInfo::GetInstance()->GetSkillLevel(pChar, 4000000, &pEntry2, 0, 0, 0, 0);
	if (nSLV1)
		nACC += pEntry1->GetLevelData(nSLV1)->m_nX;
	if (nSLV2)
	{
		nACC += pEntry2->GetLevelData(nSLV2)->m_nX;
		nEVA += pEntry2->GetLevelData(nSLV2)->m_nY;
	}

	int nAttackType = 0, nACCInc = 0, nPADInc = 0;
	if (nJobType == 3 || nJob / 10 == 41 || nJob / 10 == 52)
		nAttackType = 1;
	if (ItemInfo::GetWeaponMastery(pChar, pWeapon ? pWeapon->nItemID : 0, 0, nAttackType, &nACCInc, &nPADInc))
	{
		nACC += nACCInc;
		nPAD += nPADInc;
	}

	nPAD = std::min(1999, std::max(nPAD, 0));
	nPDD = std::min(1999, std::max(nPDD, 0));
	nMAD = std::min(1999, std::max(nMAD, 0));
	nMDD = std::min(1999, std::max(nMDD, 0));
	nACC = std::min(999, std::max(nACC, 0));
	nEVA = std::min(999, std::max(nEVA, 0));
	nSpeed = std::min(140, std::max(nSpeed, 0));
	nJump = std::min(123, std::max(nJump, 0));
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
	CHECK_TS_NORMAL(MagicGuard);
	CHECK_TS_NORMAL(DarkSight);
	CHECK_TS_NORMAL(Booster);
	CHECK_TS_NORMAL(PowerGuard);
	CHECK_TS_NORMAL(MaxHP);
	CHECK_TS_NORMAL(MaxMP);
	CHECK_TS_NORMAL(Invincible);
	CHECK_TS_NORMAL(SoulArrow);
	CHECK_TS_NORMAL(Stun);
	CHECK_TS_NORMAL(Poison);
	CHECK_TS_NORMAL(Seal);
	CHECK_TS_NORMAL(Darkness);
	CHECK_TS_NORMAL(ComboCounter);
	CHECK_TS_NORMAL(WeaponCharge);
	CHECK_TS_NORMAL(DragonBlood);
	CHECK_TS_NORMAL(HolySymbol);
	CHECK_TS_NORMAL(MesoUp);
	CHECK_TS_NORMAL(ShadowPartner);
	CHECK_TS_NORMAL(PickPocket);
	CHECK_TS_NORMAL(MesoGuard);
	CHECK_TS_NORMAL(Thaw);
	CHECK_TS_NORMAL(Weakness);
	CHECK_TS_NORMAL(Curse);
	CHECK_TS_NORMAL(Slow);
	CHECK_TS_NORMAL(Morph);
	CHECK_TS_NORMAL(Ghost);
	CHECK_TS_NORMAL(Regen);
	CHECK_TS_NORMAL(BasicStatUp);
	CHECK_TS_NORMAL(Stance);
	CHECK_TS_NORMAL(SharpEyes);
	CHECK_TS_NORMAL(ManaReflection);
	CHECK_TS_NORMAL(Attract);
	CHECK_TS_NORMAL(SpiritJavelin);
	CHECK_TS_NORMAL(Infinity);
	CHECK_TS_NORMAL(HolyShield);
	CHECK_TS_NORMAL(HamString);
	CHECK_TS_NORMAL(Blind);
	CHECK_TS_NORMAL(Concentration);
	CHECK_TS_NORMAL(BanMap);
	CHECK_TS_NORMAL(MaxLevelBuff);
	CHECK_TS_NORMAL(Barrier);
	CHECK_TS_NORMAL(ReverseInput);
	CHECK_TS_NORMAL(MesoUpByItem);
	CHECK_TS_NORMAL(ItemUpByItem);
	CHECK_TS_NORMAL(RespectPImmune);
	CHECK_TS_NORMAL(RespectMImmune);
	CHECK_TS_NORMAL(DefenseAtt);
	CHECK_TS_NORMAL(DefenseState);

	oPacket->Encode1((int)nDefenseAtt_);
	oPacket->Encode1((int)nDefenseState_);
	//oPacket->Encode1((int)nPVPDamage);

	for (int i = 0; i < 6; ++i)
	{
		if (flag & TemporaryStat::TS_Flag(TemporaryStat::TS_EnergyCharged + i))
			(this->*ms_aEncoder[i])(oPacket, flag);
	}

	WvsLogger::LogRaw(WvsLogger::LEVEL_INFO, "Encode Local TS : \n");
	oPacket->Print();
}

void SecondaryStat::EncodeForRemote(OutPacket * oPacket, TemporaryStat::TS_Flag & flag)
{
	flag.Encode(oPacket);
	CHECK_TS_REMOTE_N(Speed, 1);
	CHECK_TS_REMOTE_N(ComboCounter, 1);
	CHECK_TS_REMOTE_R(WeaponCharge, 4);
	CHECK_TS_REMOTE_R(Stun, 4);
	CHECK_TS_REMOTE_R(Darkness, 4);
	CHECK_TS_REMOTE_R(Seal, 4);
	CHECK_TS_REMOTE_R(Weakness, 4);
	CHECK_TS_REMOTE_R(Curse, 4);
	CHECK_TS_REMOTE_N(Poison, 2);
	CHECK_TS_REMOTE_R(Poison, 4);
	CHECK_TS_REMOTE_N(Morph, 2);
	CHECK_TS_REMOTE_N(Ghost, 2);
	CHECK_TS_REMOTE_R(Attract, 4);
	CHECK_TS_REMOTE_R(SpiritJavelin, 4);
	CHECK_TS_REMOTE_R(BanMap, 4);
	CHECK_TS_REMOTE_R(Barrier, 4);
	CHECK_TS_REMOTE_R(ReverseInput, 4);
	CHECK_TS_REMOTE_R(RespectPImmune, 4);
	CHECK_TS_REMOTE_R(RespectMImmune, 4);
	CHECK_TS_REMOTE_R(DefenseAtt, 4);
	CHECK_TS_REMOTE_R(DefenseState, 4);

	oPacket->Encode1((char)nDefenseAtt_);
	oPacket->Encode1((char)nDefenseState_);

	for (int i = 0; i < 6; ++i)
	{
		if (flag & TemporaryStat::TS_Flag(TemporaryStat::TS_EnergyCharged + i))
			(this->*ms_aEncoder[i])(oPacket, flag);
	}
	//RideVechicle
}

void SecondaryStat::EncodeEnergyCharged(OutPacket *oPacket, TemporaryStat::TS_Flag &flag)
{
}

void SecondaryStat::EncodeDash_Speed(OutPacket *oPacket, TemporaryStat::TS_Flag &flag)
{
}

void SecondaryStat::EncodeDash_Jump(OutPacket *oPacket, TemporaryStat::TS_Flag &flag)
{
}

void SecondaryStat::EncodeRideVehicle(OutPacket *oPacket, TemporaryStat::TS_Flag &flag)
{
	oPacket->Encode4(yRideVehicle_ && xRideVehicle_ ? yRideVehicle_ : xRideVehicle_);
	oPacket->Encode4(rRideVehicle_);
	oPacket->Encode4(0);
}

void SecondaryStat::EncodePartyBooster(OutPacket *oPacket, TemporaryStat::TS_Flag &flag)
{
}

void SecondaryStat::EncodeGuidedBullet(OutPacket *oPacket, TemporaryStat::TS_Flag &flag)
{
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

void SecondaryStat::ResetByTime(User* pUser, unsigned int tCur)
{
	std::vector<int> aSkillResetReason;
	auto &pSS = pUser->GetSecondaryStat();
	std::lock_guard<std::recursive_mutex> lock(pSS->m_mtxLock);
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

	for (auto iter = m_mCooltimeOver.begin(); iter != m_mCooltimeOver.end();)
		if (tCur > iter->second) 
		{
			pUser->SendSkillCooltimeSet(iter->first, 0);
			iter = m_mCooltimeOver.erase(iter);
		}
		else
			++iter;
}

void SecondaryStat::ResetAll(User* pUser)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);	
	for (auto& setFlag : m_mSetByTS)
		*(setFlag.second.second[2]) = 0;
	ResetByTime(pUser, GameDateTime::GetTime());
}