#pragma once
#include "Field.h"

class Field_GuildBoss : public Field
{
	int m_nYMin = 0,
		m_nYMax = 0,
		m_nFall = 0,
		m_nRise = 0,
		m_nHealMin = 0,
		m_nHealMax = 0,
		m_nState = 0,
		m_nY = 0;
public:
	Field_GuildBoss(void *pData, int nFieldID);
	~Field_GuildBoss();
	void OnPacket(User *pUser, InPacket *iPacket);
	void ResetState();
	void OnPullyHit();
	void OnEnter(User *pUser);
	void ChangePullyState(int nState);
	void SendPullyStateChange(int nState);
	void SendHealerMove();
	void Update();
};

