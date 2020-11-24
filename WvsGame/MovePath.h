#pragma once
#include <list>
class InPacket;
class OutPacket;

struct MovePath
{
	enum MoveActionType
	{
		eMoveAction_WALK = 1,
		eMoveAction_MOVE = 1,
		eMoveAction_STAND = 2,
		eMoveAction_JUMP = 3,
		eMoveAction_ALERT = 4,
		eMoveAction_PRONE = 5,
		eMoveAction_FLY = 6,
		eMoveAction_LADDER = 7,
		eMoveAction_ROPE = 8,
		eMoveAction_DEAD = 9,
		eMoveAction_SIT = 0x0A,
		eMoveAction_STAND0 = 0x0B,
		eMoveAction_HUNGRY = 0x0C,
		eMoveAction_REST0 = 0x0D,
		eMoveAction_REST1 = 0x0E,
		eMoveAction_HANG = 0x0F,
		eMoveAction_CHASE = 0x10,
		eMoveAction_NO = 0x11,
	};

	struct ELEM
	{
		short x = 0, y = 0, vx = 0, vy = 0, offsetX = 0, offsetY = 0, fh = 0, fhFootStart = 0;
		char bMoveAction = 0, bStat = 0;
		int tElapse = 0, nAttr = 0;
	};
	short m_x, m_y, m_vx, m_vy;
	int m_fhLast, m_tEncodedGatherDuration;

	std::list<ELEM> m_lElem;

	MovePath();
	~MovePath();

	void Decode(InPacket* iPacket);
	void Encode(OutPacket* oPacket);
	
};

