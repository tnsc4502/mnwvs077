#include "AttackInfo.h"

int AttackInfo::GetDamagedMobCount()
{
	return (this->m_bDamageInfoFlag >> 4 & 0xF);
}

int AttackInfo::GetDamageCountPerMob()
{
	return this->m_bDamageInfoFlag & 0xF;
}
