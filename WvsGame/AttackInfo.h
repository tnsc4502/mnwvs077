#pragma once
#include <map>
#include <vector>

class Mob;

struct AttackInfo
{
	struct DamageInfo
	{
		Mob *pMob;
		const static int MAX_DAMAGE_COUNT = 16;
		int anDamageClient[MAX_DAMAGE_COUNT] = { 0 },
			anDamageSrv[MAX_DAMAGE_COUNT] = { 0 };

		bool abDamageCriticalClient[MAX_DAMAGE_COUNT] = { false },
			abDamageCriticalSrv[MAX_DAMAGE_COUNT] = { false };
	};

	std::map<int, DamageInfo> m_mDmgInfo;
	int m_bAttackInfoFlag;
	int GetDamagedMobCount();
	int GetDamageCountPerMob();

	char 
		m_bCheckExJablinResult,
		m_bAddAttackProc,
		m_bFieldKey,
		m_bEvanForceAction;

	int 
		m_nX = 0, 
		m_nY = 0,
		m_nType = 0,
		m_nAttackType = 0,
		m_nAttackSpeed = 0,
		m_tLastAttackTime = 0,
		m_nSoulArrow = 0,
		m_nWeaponItemID = 0,
		m_nBulletItemID = 0,
		m_nWeaponType = 0,
		m_nShootRange = 0,
		m_nAttackActionType = 0,
		m_nDisplay = 0,
		m_nDamagePerMob = 0,
		m_nOption = 0,
		m_nAction = 0,
		m_apMinion = 0,
		m_nSkillID = 0,
		m_nSLV = 0,
		m_dwCRC = 0,
		m_nSlot = 0,
		m_nCsStar = 0,
		m_tKeyDown = 0,
		m_pGrenade = 0,
		m_nFinalAttack = 0
		;

};

