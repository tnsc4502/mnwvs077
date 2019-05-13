#pragma once
#include "WvsUnified.h"

struct GuildData;

class GuildDBAccessor
{
private:
	GuildDBAccessor();
	~GuildDBAccessor();

public:
	static GuildDBAccessor* GetInstance();

	int GetGuildIDCounter();
	void CreateNewGuild(void* pGuild, int nWorldID);
	void JoinGuild(void *pMemberData, int nCharacterID, int nGuildID, int nWorldID);
	void WithdrawGuild(int nCharacterID, int nGuildID, int nWorldID);
	void RemoveGuild(int nGuildID, int nWorldID);

	int LoadGuildID(int nCharacterID);
	void* LoadGuildByCharID(int nCharacterID);
	void* LoadGuild(int nGuildID);
	void UpdateGuild(void *pGuild, int nWorldID);
	void UpdateGuildMember(void *pMemberData, int nCharacterID, int nGuildID, int nWorldID);
	std::vector<void*> LoadAllGuild(int nWorldID);
};

