#pragma once
class MobStat
{
#define ADD_MOB_STAT(name, flag) const int MS_##name = 1 << flag; int n##name = 0, r##name = 0, t##name = 0;
	int nFlagSet = 0;

public:
	struct BurnedInfo
	{
		int nCharacterID = 0, nSkillID = 0, nDamage = 0;
	};

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
	ADD_MOB_STAT(PImmune, 18);
	ADD_MOB_STAT(MImmune, 19);
	ADD_MOB_STAT(Doom, 16);
	ADD_MOB_STAT(Web, 17);
	//No 20
	ADD_MOB_STAT(HardSkin, 21);
	ADD_MOB_STAT(Ambush, 22);
	//No 23
	ADD_MOB_STAT(Venom, 24);
	ADD_MOB_STAT(Blind, 25);
	ADD_MOB_STAT(SealSkill, 26);
	ADD_MOB_STAT(Dazzle, 27);
	ADD_MOB_STAT(Burned, 28);

	MobStat();
	~MobStat();
};

