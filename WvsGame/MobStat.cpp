#include "MobStat.h"
#include "..\WvsLib\Net\OutPacket.h"

MobStat::MobStat()
{
}


MobStat::~MobStat()
{
}

#define CHECK_MOB_FLAG(f_) if(nSet & MS_##f_) { \
oPacket->Encode2(n##f_); \
oPacket->Encode4(r##f_); \
oPacket->Encode2((t##f_ - tCur) / 500); \
}\

#define CHECK_MOB_STAT_TIMEOUT(f_) if(n##f_ > 0 && tCur - t##f_ > 0) { \
nSet |= MS_##f_; \
n##f_ = 0; \
r##f_ = 0; \
t##f_ = 0; \
nFlagSet &= ~(MS_##f_);\
}

void MobStat::EncodeTemporary(int nSet, OutPacket *oPacket, int tCur)
{
	CHECK_MOB_FLAG(PAD);
	CHECK_MOB_FLAG(PDD);
	CHECK_MOB_FLAG(MAD);
	CHECK_MOB_FLAG(MDD);
	CHECK_MOB_FLAG(ACC);
	CHECK_MOB_FLAG(EVA);
	CHECK_MOB_FLAG(Speed);
	CHECK_MOB_FLAG(Stun);
	CHECK_MOB_FLAG(Freeze);
	CHECK_MOB_FLAG(Poison);
	CHECK_MOB_FLAG(Seal);
	CHECK_MOB_FLAG(Darkness);
	CHECK_MOB_FLAG(PowerUp);
	CHECK_MOB_FLAG(MagicUp);
	CHECK_MOB_FLAG(PGuardUp);
	CHECK_MOB_FLAG(MGuardUp);
	CHECK_MOB_FLAG(PImmune);
	CHECK_MOB_FLAG(MImmune);
	CHECK_MOB_FLAG(Doom);
	CHECK_MOB_FLAG(Web);
	CHECK_MOB_FLAG(HardSkin);
	CHECK_MOB_FLAG(Ambush);
	CHECK_MOB_FLAG(Venom);
	CHECK_MOB_FLAG(Blind);
	CHECK_MOB_FLAG(SealSkill);
	CHECK_MOB_FLAG(Dazzle);
	if (nSet & MS_Burned)
	{
		oPacket->Encode4(1);
		oPacket->Encode4(burnedInfo.nCharacterID);
		oPacket->Encode4(burnedInfo.nSkillID);
		oPacket->Encode4(burnedInfo.nDamage);
	}
}

int MobStat::ResetTemporary(int tCur)
{
	int nSet = 0;
	CHECK_MOB_STAT_TIMEOUT(PAD);
	CHECK_MOB_STAT_TIMEOUT(PDD);
	CHECK_MOB_STAT_TIMEOUT(MAD);
	CHECK_MOB_STAT_TIMEOUT(MDD);
	CHECK_MOB_STAT_TIMEOUT(ACC);
	CHECK_MOB_STAT_TIMEOUT(EVA);
	CHECK_MOB_STAT_TIMEOUT(Speed);
	CHECK_MOB_STAT_TIMEOUT(Stun);
	CHECK_MOB_STAT_TIMEOUT(Freeze);
	CHECK_MOB_STAT_TIMEOUT(Poison);
	CHECK_MOB_STAT_TIMEOUT(Seal);
	CHECK_MOB_STAT_TIMEOUT(Darkness);
	CHECK_MOB_STAT_TIMEOUT(PowerUp);
	CHECK_MOB_STAT_TIMEOUT(MagicUp);
	CHECK_MOB_STAT_TIMEOUT(PGuardUp);
	CHECK_MOB_STAT_TIMEOUT(MGuardUp);
	CHECK_MOB_STAT_TIMEOUT(PImmune);
	CHECK_MOB_STAT_TIMEOUT(MImmune);
	CHECK_MOB_STAT_TIMEOUT(Doom);
	CHECK_MOB_STAT_TIMEOUT(Web);
	CHECK_MOB_STAT_TIMEOUT(HardSkin);
	CHECK_MOB_STAT_TIMEOUT(Ambush);
	CHECK_MOB_STAT_TIMEOUT(Venom);
	CHECK_MOB_STAT_TIMEOUT(Blind);
	CHECK_MOB_STAT_TIMEOUT(SealSkill);
	CHECK_MOB_STAT_TIMEOUT(Dazzle);

	return nSet;
}
