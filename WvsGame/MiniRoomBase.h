#pragma once
#include <atomic>
#include <mutex>
#include <map>
#include <string>

class User;
class InPacket;

class MiniRoomBase
{
	static std::atomic<int> ms_nSNCounter;
	static std::map<int, MiniRoomBase*> ms_mMiniRoom;

	std::recursive_mutex m_mtxMiniRoomLock;

	static const int MAX_USER_NUM = 8;
	User* m_pUsers[MAX_USER_NUM];

	int m_nMiniRoomSN = 0,
		m_nMaxUsers = 0,
		m_nCurUsers = 0,
		m_nType = 0;

	bool m_bOpened = false,
		m_bCloseRequest = false,
		m_bTournament = false,
		m_bGameOn = false;

	std::string m_sTitle, m_sPassword;

public:
	enum MiniRoomType
	{
		e_MiniRoom_Omok = 1,
		e_MiniRoom_MemoryGame = 2, 
		e_MiniRoom_TradingRoom = 3,
		e_MiniRoom_PersonalShop = 4,
		e_MiniRoom_EntrustedShop = 4,
	};

	enum MiniRoomRequest
	{
		rq_MiniRoom_MRCreate = 0x00,
		rq_MiniRoom_InviteBase = 0x02,
		rq_MiniRoom_MRInviteResult = 0x03,
		rq_MiniRoom_MREnter = 0x04,
		rq_MiniRoom_Chat = 0x06,
		rq_MiniRoom_LeaveBase = 0x0A,
		rq_MiniRoom_BalloonBase = 0x0B,
	};

	MiniRoomBase(int nMaxUsers);
	~MiniRoomBase();
	int GetType();

	virtual void OnPacket(User *pUser, InPacket *iPacket) = 0;
	void OnPacketBase(int nType, User *pUser, InPacket *iPacket);
	void OnCreateBase(User *pUser, InPacket *iPacket, int nRound);

	static MiniRoomBase* Create(User *pUser, int nMiniRoomType, InPacket *iPacket, bool bTournament, int nRound);
	static MiniRoomBase* MiniRoomFactory(int nMiniRoomType, InPacket *iPacket, bool bTournament);
};

