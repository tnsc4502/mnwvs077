#pragma once
#include <atomic>
#include <mutex>
#include <map>
#include <string>

class User;
class InPacket;
class OutPacket;

class MiniRoomBase
{
protected:
	static std::atomic<int> ms_nSNCounter;
	static std::mutex ms_mtxMiniRoomLock;
	static std::map<int, MiniRoomBase*> ms_mMiniRoom;
	static const int MAX_USER_NUM = 8;

	std::recursive_mutex m_mtxMiniRoomLock;

	User* m_apUser[MAX_USER_NUM];
	int m_anLeaveRequest[MAX_USER_NUM],
		m_anReservedTime[MAX_USER_NUM],
		m_anReserved[MAX_USER_NUM];

	int m_nMiniRoomSN = 0,
		m_nMaxUsers = 0,
		m_nCurUsers = 0,
		m_nType = 0,
		m_nRound = 0;

	bool m_bOpened = false,
		m_bCloseRequest = false,
		m_bTournament = false,
		m_bGameOn = false,
		m_bPrivate = false;

	std::string m_sTitle, m_sPassword;

public:
	enum MiniRoomType
	{
		e_MiniRoom_Omok = 1,
		e_MiniRoom_MemoryGame = 2, 
		e_MiniRoom_TradingRoom = 3,
		e_MiniRoom_PersonalShop = 4,
		e_MiniRoom_EntrustedShop = 5,
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

	enum MiniRoomResult
	{
		res_MiniRoom_MRInviteSuccess = 0x02,
		res_MiniRoom_MRInviteFailed = 0x03,
		res_MiniRoom_MREnter = 0x04,
		res_MiniRoom_MRCreateResult = 0x05,
	};

	enum MiniRoomCreateResult
	{
		res_Create_Success = 0x00,
		res_Create_Failed_UnableToProcess = 0x03,
	};

	enum MiniRoomInviteResult
	{
		res_Invite_InvalidUser = 0x01,
		res_Invite_UnableToProcess = 0x02,
	};

	MiniRoomBase(int nMaxUsers);
	~MiniRoomBase();
	int GetType() const;
	int GetMaxUsers() const;
	int GetCurUsers() const;
	int GetRound() const;
	void Broadcast(OutPacket *oPacket, User *pExcept);
	void SetTitle(const std::string& sTitle);
	void SetPassword(const std::string& sPassword);
	bool CheckPassword(const std::string& sPassword);
	virtual bool IsEmployer(User *pUser);
	virtual bool IsEntrusted() const;
	virtual int GetEmployeeTemplateID() const;
	const std::string& GetEmployeeName() const;
	void RemoveMiniRoom();

	virtual void OnPacket(User *pUser, int nType, InPacket *iPacket) = 0;
	virtual void Encode(OutPacket *oPacket) = 0;
	virtual void Release() = 0;
	unsigned char FindEmptySlot(int nCharacterID);
	User* FindUser(User *pUser);
	int FindUserSlot(User *pUser);
	void OnPacketBase(User *pUser, int nType, InPacket *iPacket);
	unsigned char OnCreateBase(User *pUser, InPacket *iPacket, int nRound);
	void OnInviteBase(User *pUser, InPacket *iPacket);
	void OnLeaveBase(User *pUser, InPacket *iPacket);
	virtual void OnLeave(User *pUser, int nLeaveType);
	void CloseRquest(User *pUser, int nLeaveType, int nLeaveType2);
	void DoLeave(int nIdx, int nLeaveType, bool bBroadcast);
	unsigned char OnEnterBase(User *pUser, InPacket *iPacket);
	virtual int IsAdmitted(User *pUser, InPacket *iPacket, bool bOnCreate);

	//Encode
	void EncodeAvatar(int nIdx, OutPacket *oPacket);
	virtual void EncodeEnter(User *pUser, OutPacket *oPacket);
	virtual void EncodeEnterResult(User *pUser, OutPacket *oPacket);
	virtual void EncodeLeave(User *pUser, OutPacket *oPacket);

	//Static
	static void Enter(User *pUser, int nSN, InPacket *iPacket, bool bTournament);
	static MiniRoomBase* Create(User *pUser, int nMiniRoomType, InPacket *iPacket, bool bTournament, int nRound);
	static MiniRoomBase* MiniRoomFactory(int nMiniRoomType, InPacket *iPacket, bool bTournament);
	static MiniRoomBase* GetMiniRoom(int nSN);
};

