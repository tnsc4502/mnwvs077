#pragma once
#include <map>
#include <mutex>
#include <vector>

#include "BasicStat.h"
#include "TemporaryStat.h"

#define ADD_TEMPORARY(stat) int nLv##stat##_ = 0, n##stat##_ = 0, t##stat##_ = 0, r##stat##_ = 0, b##stat##_ = 0, x##stat##_ = 0, y##stat##_ = 0;

class User;
struct GA_Character;
class OutPacket;
class InPacket;

class SecondaryStat : public BasicStat
{
public:
	//m[TS Flag, [tDuration, a[nValue, rValue, tValue, nSLV]]]
	std::map<unsigned int, std::pair<long long int, std::vector<int*>>> m_mSetByTS;
	std::map<int, unsigned int> m_mCooltimeOver;
	TemporaryStat::TS_Flag m_tsFlagSet;
	std::recursive_mutex m_mtxLock;

	ADD_TEMPORARY(PAD);
	ADD_TEMPORARY(PDD);
	ADD_TEMPORARY(MAD);
	ADD_TEMPORARY(MDD);
	ADD_TEMPORARY(ACC);
	ADD_TEMPORARY(EVA);
	ADD_TEMPORARY(Craft);
	ADD_TEMPORARY(Speed);
	ADD_TEMPORARY(Jump);
	ADD_TEMPORARY(MagicGuard);
	ADD_TEMPORARY(DarkSight);
	unsigned int mDarkSight_;

	ADD_TEMPORARY(Booster);
	ADD_TEMPORARY(PowerGuard);
	ADD_TEMPORARY(MaxHP);
	ADD_TEMPORARY(MaxMP);
	ADD_TEMPORARY(Invincible);
	ADD_TEMPORARY(SoulArrow);
	ADD_TEMPORARY(Stun);
	ADD_TEMPORARY(Poison);
	ADD_TEMPORARY(Seal);
	ADD_TEMPORARY(Darkness);
	ADD_TEMPORARY(Summon);
	ADD_TEMPORARY(ComboCounter);
	ADD_TEMPORARY(WeaponCharge);
	ADD_TEMPORARY(DragonBlood);
	ADD_TEMPORARY(HolySymbol);
	ADD_TEMPORARY(MesoUp);
	ADD_TEMPORARY(ShadowPartner);
	ADD_TEMPORARY(PickPocket);
	ADD_TEMPORARY(MesoGuard);
	ADD_TEMPORARY(Thaw);
	ADD_TEMPORARY(Weakness);
	ADD_TEMPORARY(Curse);
	ADD_TEMPORARY(Slow);
	ADD_TEMPORARY(Morph);
	ADD_TEMPORARY(Regen);
	ADD_TEMPORARY(BasicStatUp);
	ADD_TEMPORARY(Stance);
	ADD_TEMPORARY(SharpEyes);
	ADD_TEMPORARY(ManaReflection);
	ADD_TEMPORARY(Attract);
	ADD_TEMPORARY(SpiritJavelin);
	ADD_TEMPORARY(Infinity);
	ADD_TEMPORARY(HolyShield);
	ADD_TEMPORARY(HamString);
	ADD_TEMPORARY(Blind);
	ADD_TEMPORARY(Concentration);
	ADD_TEMPORARY(BanMap);
	int mBanMap = 0;

	ADD_TEMPORARY(MaxLevelBuff);
	ADD_TEMPORARY(MesoUpByItem);
	ADD_TEMPORARY(Ghost);
	ADD_TEMPORARY(Barrier);
	ADD_TEMPORARY(ReverseInput);
	ADD_TEMPORARY(ItemUpByItem);
	ADD_TEMPORARY(RespectPImmune);
	ADD_TEMPORARY(RespectMImmune);
	ADD_TEMPORARY(DefenseAtt);
	ADD_TEMPORARY(DefenseState);

	ADD_TEMPORARY(EnergyCharged);
	ADD_TEMPORARY(Dash_Speed);
	ADD_TEMPORARY(Dash_Jump);
	ADD_TEMPORARY(RideVehicle);
	ADD_TEMPORARY(PartyBooster);
	ADD_TEMPORARY(GuidedBullet);
	ADD_TEMPORARY(Undead);
	ADD_TEMPORARY(RideVehicleExpire);

	int nPDD,
		nMDD,
		nMAD,
		nPAD,
		nACC,
		nEVA,
		nSpeed,
		nJump,
		nCraft;

public:
	SecondaryStat();
	~SecondaryStat();

	void SetFrom(GA_Character* pChar, BasicStat* pBS);
	void EncodeForLocal(OutPacket *oPacket, TemporaryStat::TS_Flag& flag);
	void EncodeForRemote(OutPacket *oPacket, TemporaryStat::TS_Flag& flag);
	void EncodeEnergyCharged(OutPacket *oPacket, TemporaryStat::TS_Flag& flag);
	void EncodeDash_Speed(OutPacket *oPacket, TemporaryStat::TS_Flag& flag);
	void EncodeDash_Jump(OutPacket *oPacket, TemporaryStat::TS_Flag& flag);
	void EncodeRideVehicle(OutPacket *oPacket, TemporaryStat::TS_Flag& flag);
	void EncodePartyBooster(OutPacket *oPacket, TemporaryStat::TS_Flag& flag); 
	void EncodeGuidedBullet(OutPacket *oPacket, TemporaryStat::TS_Flag& flag); 
	bool EnDecode4Byte(TemporaryStat::TS_Flag& flag);
	void ResetByTime(User* pUser, unsigned int tCur);
	void ResetAll(User* pUser);

private:
	static decltype(&SecondaryStat::EncodeForLocal) ms_aEncoder[];
};

