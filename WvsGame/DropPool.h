#pragma once
#include <atomic>
#include <mutex>
#include <map>
#include <vector>
#include "FieldRect.h"

class Drop;
class User;
class Reward;
class Field;
class InPacket;
class Pet;

class DropPool
{
	std::mutex m_mtxDropPoolLock;
	std::atomic<int> m_nDropIdCounter;
	std::map<int, Drop*> m_mDrop;
	bool m_bDropEverlasting = false;
	int m_tLastExpire;
	Field *m_pField;

public:

	const Drop* GetDrop(int nDropID);
	void Create(Reward *reward, unsigned int dwOwnerID, unsigned int dwOwnPartyID, int nOwnType, unsigned int dwSourceID, int x1, int y1, int x2, int y2, int tDelay, int bAdmin, int nPos, bool bByPet);
	void OnEnter(User* pUser);
	void OnPacket(User* pUser, int nType, InPacket* iPacket);
	void OnPickUpRequest(User* pUser, InPacket *iPacket, Pet *pPet);
	std::vector<Drop*> FindDropInRect(const FieldRect& rc, int tTimeAfter);
	void Remove(int nID, int tDelay);
	void TryExpire(bool bRemoveAll);
	DropPool(Field* pField);
	~DropPool();
};

