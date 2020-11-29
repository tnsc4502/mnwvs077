#pragma once
#include <map>
#include <vector>
#include <mutex>
#include "..\WvsLib\Memory\ZMemory.h"
#include "FieldPoint.h"

class TownPortal;
class Field;
class User;

class TownPortalPool
{
	Field *m_pField = nullptr;
	std::vector<FieldPoint> m_aTownPortal;
	std::map<int, ZUniquePtr<TownPortal>> m_mTownPortal;
	std::mutex m_mtxLock;

public:
	TownPortalPool();
	~TownPortalPool();
	void OnEnter(User *pUser);

	void SetField(Field *pField);
	void AddTownPortalPos(FieldPoint pos);
	FieldPoint GetTownPortalPos(int nIdx);
	const TownPortal* GetTownPortal(int nCharacterID);
	bool CreateTownPortal(int nCharacterID, int nX, int nY, unsigned int tEnd);
	void RemoveTownPortal(int nCharacterID);
	void Update(unsigned int tCur);
};

