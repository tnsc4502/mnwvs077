#pragma once
#include "FieldObj.h"
#include "FieldRect.h"

class OutPacket;

class AffectedArea : public FieldObj
{
	friend class AffectedAreaPool;

	int	m_nOwnerID = 0,
		m_nSkillID = 0,
		m_nSLV = 0,
		m_tStart = 0,
		m_tEnd = 0;

	bool m_bMobSkill = false, m_bSmoke = false;
	FieldRect m_rcAffectedArea;

public:
	AffectedArea();
	~AffectedArea();

	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);

	int GetSkillID() const;
	int GetSkillLevel() const;
	int GetOwnerID() const;
};

