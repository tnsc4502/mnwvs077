#pragma once
#include <map>
#include <string>
#include <mutex>
#include <atomic>
#include <vector>

class InPacket;
class OutPacket;
class User;

class GuildMan
{
public:
	enum GuildRequest
	{
		rq_Guild_Load = 0x00,
		rq_Guild_Create = 0x01,
		rq_Guild_Invite = 0x05,
		rq_Guild_Join = 0x06,
	};

	enum GuildResult
	{
		res_Guild_Load = 0x00,
		res_Guild_Create = 0x01,
		res_Guild_Invite = 0x05,
		res_Guild_Update = 0x1A,
		res_Guild_Failed_Already_In_Guild = 40,
		res_Guild_Join = 39,
	};

	struct MemberData
	{
		std::string sCharacterName;
		int nLevel = 0, 
			nJob = 0, 
			nGrade = 0, 
			nContribution = 0;

		bool bOnline = false;

		void Set(const std::string& strName, int nLevel, int nJob, bool bOnline);
		void Encode(OutPacket *oPacket) const;
		void Decode(InPacket *iPacket);
	};

	struct GuildData
	{
		std::string sGuildName, sNotice;
		std::vector<std::string> asGradeName;
		std::vector<int> anCharacterID;
		std::vector<MemberData> aMemberData;

		int nGuildID = 0,
			nPoint = 0,
			nMaxMemberNum = 0,
			nMark = 0,
			nMarkBg = 0,
			nMarkColor = 0,
			nMarkBgColor = 0,
			nLoggedInUserCount = 0;

		void Encode(OutPacket *oPacket) const;
		void Decode(InPacket *iPacket);
	};

private:
	std::recursive_mutex m_mtxGuildLock;
	std::atomic<int> m_atiGuildIDCounter;

	GuildMan();
	~GuildMan();

	std::map<int, int> m_mCharIDToGuildID;
	std::map<int, GuildData*> m_mGuild;
	std::map<std::string, GuildData*> m_mNameToGuild;

public:
	static GuildMan* GetInstance();
	int GetGuildIDByCharID(int nCharacterID);
	GuildData *GetGuildByCharID(int nCharacterID);
	GuildData *GetGuild(int nGuildID);
	GuildData *GetGuildByName(const std::string& strName);
	int FindUser(int nCharacterID, GuildData *pData);
	void OnLeave(User *pUser);
	void Broadcast(OutPacket *oPacket, const std::vector<int>& anMemberID, int nPlusOne);
	void OnPacket(InPacket *iPacket);
	void OnGuildReuqest(User *pUser, InPacket *iPacket);
	void OnGuildLoadDone(InPacket *iPacket);
	void OnCreateNewGuildDone(InPacket *iPacket);

	void OnGuildInviteRequest(User *pUser, InPacket *iPacket);
	void OnCreateNewGuildRequest(User *pUser, const std::string& strGuildName, const std::vector<std::pair<int, MemberData>>& aMember);

	void OnJoinGuildReqest(User *pUser, InPacket *iPacket);
	void OnJoinGuildDone(InPacket *iPacket);

	void MakeGuildUpdatePacket(OutPacket *oPacket, GuildData *pGuild);

	///==========================CENTER=================================
#ifdef _WVSCENTER
	void SendToAll(GuildData* pGuild, OutPacket *oPacket);
	void LoadGuild(InPacket *iPacket, OutPacket *oPacket);
	void CreateNewGuild(InPacket *iPacket, OutPacket *oPacket);
	void JoinGuild(InPacket *iPacket, OutPacket *oPacket);
#endif
};

