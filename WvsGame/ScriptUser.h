#pragma once
#include "..\WvsLib\Script\lua.h"

class User;

class ScriptUser
{
	User *m_pUser;

public:
	ScriptUser();
	~ScriptUser();

	User* GetUser();
	static ScriptUser* GetSelf(lua_State* L);
	static void DestroySelf(lua_State* L, ScriptUser* p);
	static void Register(lua_State* L);
	static int TargetChatMessage(lua_State* L);
	static int TargetNoticeMessage(lua_State* L);
	static int TargetInventory(lua_State* L);
	static int TargetQuestRecord(lua_State* L);
	static int TargetGetField(lua_State* L);
	static int TargetPlayPortalSE(lua_State* L);
	static int TargetRegisterTransferField(lua_State* L);
	static int TargetGetPosX(lua_State* L);
	static int TargetGetPosY(lua_State* L);
	static int TargetIncHP(lua_State* L);
	static int TargetIncMP(lua_State* L);
	static int TargetIncSTR(lua_State* L);
	static int TargetIncINT(lua_State* L);
	static int TargetIncDEX(lua_State* L);
	static int TargetIncLUK(lua_State* L);
	static int TargetIncSP(lua_State* L);
	static int TargetIncAP(lua_State* L);
	static int TargetIncMMP(lua_State* L);
	static int TargetIncMHP(lua_State* L); 
	static int TargetIncEXP(lua_State* L);
	static int TargetIncMoney(lua_State* L);
	static int TargetSetJob(lua_State* L);

	static int TargetGetLevel(lua_State* L);
	static int TargetGetJob(lua_State* L);
	static int TargetGetHP(lua_State* L);
	static int TargetGetMP(lua_State* L);
	static int TargetGetSTR(lua_State* L);
	static int TargetGetINT(lua_State* L);
	static int TargetGetDEX(lua_State* L);
	static int TargetGetLUK(lua_State* L);
	static int TargetGetSP(lua_State* L);
	static int TargetGetAP(lua_State* L);
	static int TargetGetMMP(lua_State* L);
	static int TargetGetMHP(lua_State* L);
	static int TargetGetEXP(lua_State* L);
	static int TargetGetMoney(lua_State* L);
	static int TargetGetHair(lua_State* L);
	static int TargetGetFace(lua_State* L);
	static int TargetGetSkin(lua_State* L);
	static int TargetGetGender(lua_State* L);
	static int TargetIsWearing(lua_State* L);
	static int TargetQuestEndEffect(lua_State* L);
	static int TargetIsGuildMember(lua_State* L);
	static int TargetIsGuildMaster(lua_State* L);
	static int TargetIsGuildSubMaster(lua_State* L);
	static int TargetGetGuildCountMax(lua_State* L);
	static int TargetIsPartyBoss(lua_State* L);
	static int TargetGetPartyMemberJob(lua_State* L);
	static int TargetGetPartyMemberLevel(lua_State* L);
	static int TargetGiveBuff(lua_State* L);
	static int TargetCancelBuff(lua_State* L);
	static int TargetGivePartyBuff(lua_State* L);
	static int TargetCancelPartyBuff(lua_State* L);
	static int TargetGetPartyID(lua_State* L);
	static int TargetGetMCTeam(lua_State* L);
	static int TargetGetName(lua_State* L);
	static int TargetCreateNewGuild(lua_State* L);
	static int TargetRemoveGuild(lua_State* L);
	static int TargetSetGuildMark(lua_State* L);
	static int TargetRemoveGuildMark(lua_State* L);
	static int TargetIsGuildMarkExist(lua_State* L);
	static int TargetIncGuildCountMax(lua_State* L);
	static int TargetIncGuildPoint(lua_State* L);
	static int TargetCheckCondition(lua_State* L);
	static int TargetMessage(lua_State* L);
	static int TargetGetMorphState(lua_State* L);
	static int TargetCanEnterGuildQuest(lua_State* L);
	static int TargetIsGuildQuestRegistered(lua_State* L);

	static int TargetException1(lua_State* L);
	static int TargetException2(lua_State* L);
};

