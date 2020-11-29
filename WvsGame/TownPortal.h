#pragma once
#include "FieldObj.h"

class TownPortal : public FieldObj
{
	friend class TownPortalPool;
	int m_nCharacterID = 0;
	unsigned int m_tEnd = 0;
public:
	TownPortal();
	~TownPortal();

	void Init(int nCharacterID, int nX, int nY, unsigned tEnd);

	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeEnterFieldPacket(OutPacket *oPacket, int nEnterType);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket, int nLeaveType);
};

