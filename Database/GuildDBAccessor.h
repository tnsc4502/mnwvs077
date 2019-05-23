#pragma once

#include <vector>
class GuildDBAccessor
{
public:

	static int GetGuildIDCounter();
	static void CreateNewGuild(void* pGuild, int nWorldID);
	static void JoinGuild(void *pMemberData, int nCharacterID, int nGuildID, int nWorldID);
	static void WithdrawGuild(int nCharacterID, int nGuildID, int nWorldID);
	static void RemoveGuild(int nGuildID, int nWorldID);
	static int LoadGuildID(int nCharacterID);
	static void* LoadGuildByCharID(int nCharacterID);
	static void* LoadGuild(int nGuildID);
	static void UpdateGuild(void *pGuild, int nWorldID);
	static void UpdateGuildMember(void *pMemberData, int nCharacterID, int nGuildID, int nWorldID);
	static std::vector<void*> LoadAllGuild(int nWorldID);
};

