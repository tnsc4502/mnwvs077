#pragma once
#include "MobTemplate.h"

class OutPacket;
class MobStat
{
public:
	struct BurnedInfo
	{
		int nCharacterID = 0, nSkillID = 0, nDamage = 0;
	};

private:
#define ADD_MOB_STAT(name, flag) const static int MS_##name = 1 << flag; int n##name##_ = 0, r##name##_ = 0; unsigned int t##name##_ = 0;
	friend class Mob;
	int nFlagSet = 0;
	BurnedInfo burnedInfo;

public:
	ADD_MOB_STAT(PAD, 0);
	ADD_MOB_STAT(PDD, 1);
	ADD_MOB_STAT(MAD, 2);
	ADD_MOB_STAT(MDD, 3);
	ADD_MOB_STAT(ACC, 4);
	ADD_MOB_STAT(EVA, 5);
	ADD_MOB_STAT(Speed, 6);
	ADD_MOB_STAT(Stun, 7);
	ADD_MOB_STAT(Freeze, 8);
	ADD_MOB_STAT(Poison, 9);
	ADD_MOB_STAT(Seal, 10);
	ADD_MOB_STAT(Darkness, 11);
	ADD_MOB_STAT(PowerUp, 12);
	ADD_MOB_STAT(MagicUp, 13);
	ADD_MOB_STAT(PGuardUp, 14);
	ADD_MOB_STAT(MGuardUp, 15);
	ADD_MOB_STAT(Doom, 16);
	ADD_MOB_STAT(Web, 17);
	ADD_MOB_STAT(PImmune, 18);
	ADD_MOB_STAT(MImmune, 19);
	ADD_MOB_STAT(Showdown, 20);
	ADD_MOB_STAT(HardSkin, 21);
	ADD_MOB_STAT(Ambush, 22);
	//No 23
	ADD_MOB_STAT(Venom, 24);
	ADD_MOB_STAT(Blind, 25);
	ADD_MOB_STAT(SealSkill, 26);
	ADD_MOB_STAT(Dazzle, 27);
	ADD_MOB_STAT(Burned, 28);

	int nPAD = 0,
		nPDD = 0,
		nMAD = 0,
		nMDD = 0,
		nACC = 0,
		nEVA = 0,
		nSpeed = 0,
		nLevel = 0;

	double dFs = 0;
	bool bInvincible = false;
	int aDamagedElemAttr[MobTemplate::MAX_DAMAGED_ELEM_ATTR] = { 0 };

	MobStat();
	~MobStat();

	void SetFrom(const MobTemplate *pTemplate);
	void EncodeTemporary(int nSet, OutPacket *oPacket, unsigned int tCur);
	int ResetTemporary(unsigned int tCur);
};

