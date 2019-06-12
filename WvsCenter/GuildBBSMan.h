#pragma once
#include <vector>

class InPacket;
class OutPacket;
class GuildBBSMan
{
public:
	struct BBSComment
	{
		int m_nSN = 0, m_nCharacterID = 0;
		long long int m_liDate = 0;
		std::string m_sComment;
	};

	struct BBSEntry
	{
		bool m_bIsNotice = false;
		int m_nEmoticon = 0, m_nCharacterID = 0, m_nEntryID = 0, m_nCommentCount;
		long long m_liDate = 0;
		std::string m_sTitle, m_sText;
		std::vector<BBSComment> m_aComment;
		void Encode(OutPacket *oPacket);
	};

	enum GuildBBSRequest
	{
		rq_GuildBBS_RegisterEntry = 0x00,
		rq_GuildBBS_DeleteEntry = 0x01,
		rq_GuildBBS_LoadList = 0x02,
		rq_GuildBBS_ViewEntry = 0x03,
		rq_GuildBBS_RegisterComment = 0x04,
		rq_GuildBBS_DeleteComment = 0x05,
	};

	enum GuildBBSResult
	{
		res_GuildBBS_LoadEntry = 6,
		res_GuildBBS_ViewEntry = 7,
	};

private:
	GuildBBSMan();
	~GuildBBSMan();

public:
	static GuildBBSMan* GetInstance();

	void OnGuildBBSRequest(void *pGuild_, InPacket *iPacket);
	void RegisterEntry(void *pGuild_, int nCharacterID, InPacket *iPacket);
	void DeleteEntry(void *pGuild_, int nCharacterID, InPacket *iPacket);
	void LoadList(void *pGuild_, int nCharacterID, InPacket *iPacket);
	void ViewEntry(void *pGuild_, int nCharacterID, int nEntryID);
	void RegisterComment(void *pGuild_, int nCharacterID, InPacket *iPacket);
	void DeleteComment(void *pGuild_, int nCharacterID, InPacket *iPacket);
};

