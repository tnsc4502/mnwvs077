#pragma once
#include "FieldObj.h"
#include <mutex>
#include <string>

class MiniRoomBase;
class OutPacket;

class Employee : public FieldObj
{
	friend class LifePool;
	MiniRoomBase* m_pMiniRoom;
	std::recursive_mutex m_mtxLock;

	int m_nEmployerID = 0, m_nTemplateID = 0;
	std::string m_sEmployerName;

public:
	Employee();
	~Employee();

	void EncodeInitData(OutPacket *oPacket);
	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *iPacket);
};

