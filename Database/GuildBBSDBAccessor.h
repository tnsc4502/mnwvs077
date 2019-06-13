#pragma once
#include <vector>
#include <string>
class GuildBBSDBAccessor
{
public:
	static int RegisterEntry(int nWorldID, int nGuildID, int nCharacterID, const std::string& sTitle, const std::string& sText, int nEmoticon, bool bNotice);
	static int QueryEntryOwner(int nWorldID, int nGuildID, int nEntryID);
	static void DeleteEntry(int nWorldID, int nGuildID, int nEntryID, int nCharacterID, bool bForce);
	static void ModifyEntry(int nWorldID, int nGuildID, int nEntryID, int nCharacterID, const std::string& sTitle, const std::string& sText, int nEmoticon, bool bForce);
	static void RegisterComment(int nWorldID, int nGuildID, int nEntryID, int nCharacterID, const std::string& sComment);
	static void DeleteComment(int nWorldID, int nGuildID, int nEntryID, int nCharacterID, int nCommentSN, bool bForce);
	static std::vector<void*> LoadList(int nWorldID, int nGuildID);
	static void* LoadEntryView(int nWorldID, int nGuildID, int nEntryID);
	static int GetNextEntryID(int nWorldID, int nGuildID);
	static int GetNextCommentSN(int nWorldID, int nGuildID, int nEntryID);
	static int QueryEntryCount(int nWorldID, int nGuildID);
	static int QueryCommentCount(int nWorldID, int nGuildID, int nEntryID);
	static bool IsNoticeExist(int nWorldID, int nGuildID);
};

