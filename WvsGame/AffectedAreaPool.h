#pragma once
#include <vector>
#include <atomic>
#include "FieldPoint.h"
#include "FieldRect.h"

class AffectedArea;
class Field;
class User;

class AffectedAreaPool
{
	std::atomic<int> m_nAffectedAreaIDCounter;
	std::vector<AffectedArea*> m_apAffectedArea;
	Field *m_pField;

public:
	AffectedAreaPool(Field *pField);
	~AffectedAreaPool();
	void OnEnter(User *pUser);

	const std::vector<AffectedArea*>& GetAffectedAreas() const;
	void InsertAffectedArea(bool bMobSkill, int nOwnerID, int nSkillID, int nSLV, unsigned int tStart, unsigned int tEnd, const FieldPoint& pt, const FieldRect& rc, bool bSmoke);
	AffectedArea* GetAffectedAreaByPoint(const FieldPoint& pt);
	void Update(unsigned int tCur);

};