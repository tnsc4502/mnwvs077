#pragma once

class OutPacket;

#define ADD_TS_FLAG(flag, value) static const int TS_##flag = value;
#define GET_TS_FLAG(flagName) TemporaryStat::TS_Flag(TemporaryStat::TS_##flagName)

struct TemporaryStat
{
	class TS_Flag {
	private:
		bool bEmpty = true;
		static const int FLAG_COUNT = 4;
		int m_aData[FLAG_COUNT], m_nFlagPos, m_nTSValue;

	public:
		TS_Flag();
		TS_Flag(int dwFlagValue);
		void Encode(OutPacket *oPacket);
		TS_Flag& operator |= (const TS_Flag& rhs);
		TS_Flag& operator ^= (const TS_Flag& rhs);
		bool operator & (const TS_Flag& rhs);
		bool IsEmpty() const;
		bool IsIndieTS() const;

		static TS_Flag GetDefault();
	};

	ADD_TS_FLAG(PAD, 0);
	ADD_TS_FLAG(PDD, 1);
	ADD_TS_FLAG(MAD, 2);
	ADD_TS_FLAG(MDD, 3);
	ADD_TS_FLAG(ACC, 4);
	ADD_TS_FLAG(EVA, 5);
	ADD_TS_FLAG(Craft, 6);
	ADD_TS_FLAG(Speed, 7);
	ADD_TS_FLAG(Jump, 8);
	ADD_TS_FLAG(MagicGuard, 9);
	ADD_TS_FLAG(DarkSight, 10);
	ADD_TS_FLAG(Booster, 11);
	ADD_TS_FLAG(PowerGuard, 12);
	ADD_TS_FLAG(MaxHP, 13);
	ADD_TS_FLAG(MaxMP, 14);
	ADD_TS_FLAG(Invincible, 15);
	ADD_TS_FLAG(SoulArrow, 16);
	ADD_TS_FLAG(Stun, 17);
	ADD_TS_FLAG(Poison, 18);
	ADD_TS_FLAG(Seal, 19);
	ADD_TS_FLAG(Darkness, 20);
	ADD_TS_FLAG(Summon, 21);
	ADD_TS_FLAG(ComboCounter, 21);
	ADD_TS_FLAG(WeaponCharge, 22);
	ADD_TS_FLAG(DragonBlood, 23);
	ADD_TS_FLAG(HolySymbol, 24);
	ADD_TS_FLAG(MesoUp, 25);
	ADD_TS_FLAG(ShadowPartner, 26);
	ADD_TS_FLAG(PickPocket, 27);
	ADD_TS_FLAG(MesoGuard, 28);
	ADD_TS_FLAG(Thaw, 29);
	ADD_TS_FLAG(Weakness, 30);
	ADD_TS_FLAG(Curse, 31);
	ADD_TS_FLAG(Slow, 32);
	ADD_TS_FLAG(Morph, 33);
	ADD_TS_FLAG(Regen, 34);
	ADD_TS_FLAG(BasicStatUp, 35);
	ADD_TS_FLAG(Stance, 36);
	ADD_TS_FLAG(SharpEyes, 37);
	ADD_TS_FLAG(ManaReflection, 38);
	ADD_TS_FLAG(Attract, 39);
	ADD_TS_FLAG(SpiritJavelin, 40);
	ADD_TS_FLAG(Infinity, 41);
	ADD_TS_FLAG(HolyShield, 42);
	ADD_TS_FLAG(HamString, 43);
	ADD_TS_FLAG(Blind, 44);
	ADD_TS_FLAG(Concentration, 45);
	ADD_TS_FLAG(BanMap, 46);
	ADD_TS_FLAG(MaxLevelBuff, 47);
	ADD_TS_FLAG(MesoUpByItem, 48);
	ADD_TS_FLAG(Ghost, 49);
	ADD_TS_FLAG(Barrier, 50);
	ADD_TS_FLAG(ReverseInput, 51);
	ADD_TS_FLAG(ItemUpByItem, 52);
	ADD_TS_FLAG(RespectPImmune, 53);
	ADD_TS_FLAG(RespectMImmune, 54);
	ADD_TS_FLAG(DefenseAtt, 55);
	ADD_TS_FLAG(DefenseState, 56);


	ADD_TS_FLAG(EnergyCharged, 57);
	ADD_TS_FLAG(Dash_Speed, 58);
	ADD_TS_FLAG(Dash_Jump, 59);
	ADD_TS_FLAG(RideVehicle, 60); 
	ADD_TS_FLAG(PartyBooster, 61);
	ADD_TS_FLAG(GuidedBullet, 62);
};

