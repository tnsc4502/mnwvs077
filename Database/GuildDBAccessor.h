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
	void* LoadGuild(int nCharacterID);
};

