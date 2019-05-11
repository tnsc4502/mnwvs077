#pragma once
#include <map>
#include <vector>

struct AttackInfo
{
	std::map<int, std::vector<long long int>> m_mDmgInfo;

	int m_bAttackInfoFlag;

	//How many monsters were damaged
	int GetDamagedMobCount();

	//How many count the monster was damaged
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
		m_nAttackSpeed = 0,
		m_tLastAttackTime = 0,
		m_nSoulArrow = 0,
		m_nWeaponType = 0,
		m_nShootRange = 0,
		m_nAttackActionType = 0,
		m_nDisplay = 0,
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

