#pragma once
#include "FieldObj.h"

class InPacket;
class OutPacket;
class User;
class SummonedPool;
class SkillEntry;

class Summoned : public FieldObj
{
public:
	enum LeaveType
	{
		eLeave_ResetByTime = 0,
		eLeave_TransferField = 2,
		eLeave_UserDead = 3,
	};

private:
	friend class SummonedPool;
	User* m_pOwner;
	const SkillEntry* m_pSkillEntry;

	bool 
		m_bFlyMob = true, 
		m_bBeforeFirstAttack = true, 
		m_bMirrored = false,
		m_bJaguarActive = true,
		m_bAttackActive = true;

	int m_nSkillID = 0, 
		m_nSLV = 0,
		m_nMoveAbility = 0,
		m_nAssitType = 0,
		m_nEnterType = 0,
		m_nMobID = 0,
		m_nLookID = 0,
		m_nBulletID = 0;

	unsigned int 
		m_tLastAttackTime = 0,
		m_tEnd = 0;

public:
	Summoned();
	~Summoned();

	int GetOwnerID() const;
	int GetSkillID() const;
	int GetSkillLevel() const;
	unsigned int GetTimeEnd() const;
	void Init(User *pUser, int nSkillID, int nSLV);
	void OnPacket(InPacket *iPacket, int nType);
	void OnMove(InPacket *iPacket);
	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
	static int GetMoveAbility(int nSkillID);
	static int GetAssitType(int nSkillID);
	void OnAttack(InPacket *iPacket);
};

