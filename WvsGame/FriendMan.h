#pragma once
#include <map>
#include <string>
#include <mutex>
#include <vector>

class User;
class InPacket;
class OutPacket;
struct GW_Friend;

class FriendMan
{
public:
	enum FriendRequest
	{
		rq_Friend_Load = 0x00,
		rq_Friend_Set = 0x01,
		rq_Friend_Accept = 0x02,
		rq_Friend_Delete = 0x03,
	};

	enum FriendResult
	{
		res_Friend_Set = 0x01,
		res_Friend_Load = 0x07,
		res_Friend_Invite = 0x09,
		res_Friend_Notify = 0x14,
	};

	enum FriendStatus
	{
		s_FriendListIsFull = -3,
		s_AlreadyInFriendList = -2,
		s_FriendNotExists = -1,
	};

	struct FriendEntry
	{
		std::recursive_mutex mtxEntryLock;
		std::vector<GW_Friend*> aFriend;
		std::vector<int> aInShop;

		int nFriendMax = 0;
		int FindIndex(int nFriendID, int nFlag = -1);

		void Encode(OutPacket *oPacket);
		void Decode(InPacket *iPacket);
	};

private:
	std::recursive_mutex m_mtxFriendLock;
	FriendMan();
	~FriendMan();
	std::map<int, FriendEntry*> m_mFriend;
	std::map<int, std::vector<std::pair<int, std::string>>> m_mFriendRequest;

public:
	static FriendMan* GetInstance();
	void OnPacket(InPacket *iPacket);
	void OnInviteRequest(InPacket *iPacket);
	void OnSetFriendDone(InPacket *iPacket);
	void OnLoadFriendDone(InPacket *iPacket);
	void OnNotify(InPacket *iPacket);
	void OnFriendRequest(User *pUser, InPacket *iPacket);
	void OnDeleteFriendRequest(User *pUser, InPacket *iPacket);
	void OnAcceptFriendRequest(User *pUser, InPacket *iPacket);
	void OnSetFriendRequest(User *pUser, InPacket *iPacket);
	void OnLeave(User *pUser);
	FriendEntry* GetFriendEntry(int nCharacterID);

	//WvsCenter
	void InsertFriendInvitationRequest(int nCharacterID, int nRequestFrom, const std::string& strNameFrom);
	const std::vector<std::pair<int, std::string>>& GetFriendInvitationRequest(int nCharacterID);
	const std::pair<int, std::string>& GetInvitationRequest(int nCharacterID, int nFriendID);
	void RemoveFriendInvitationRequest(int nCharacterID, int nFriendID);
	void RemoveFriendInvitationRequest(int nCharacterID);

	void Notify(int nCharacterID, int nFriendID, int nChannelID, bool bShop);
	void NotifyLogin(int nCharacterID, int nChannelID, bool bShop);
	void NotifyLogout(int nCharacterID);
	void NotifyFriendRequest(int nCharacterID, int nRequestFrom, const std::string& strNameFrom);
	void SetFriend(InPacket *iPacket, OutPacket *oPacket);
	void LoadFriend(InPacket *iPacket, OutPacket *oPacket);
	void AcceptFriend(InPacket *iPacket, OutPacket *oPacket);
	void DeleteFriend(InPacket *iPacket, OutPacket *oPacket);
	FriendEntry* LoadFriendEntry(int nCharacterID);
	void RemoveFriendEntry(int nCharacterID);
	bool RemoveFriend(FriendEntry *pEntry, int nFriendID);
};

