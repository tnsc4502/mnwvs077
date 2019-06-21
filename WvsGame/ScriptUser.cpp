#include "ScriptUser.h"
#include "ScriptInventory.h"
#include "ScriptQuestRecord.h"
#include "ScriptField.h"
#include "Script.h"
#include "User.h"
#include "QWUser.h"
#include "PartyMan.h"
#include "GuildMan.h"
#include "ItemInfo.h"
#include "StateChangeItem.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GA_Character.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"

ScriptUser::ScriptUser()
{
}

ScriptUser::~ScriptUser()
{
}

User * ScriptUser::GetUser()
{
	return m_pUser;
}

ScriptUser * ScriptUser::GetSelf(lua_State * L)
{
	int nUserID = (int)luaL_checkinteger(L, 1);
	auto pUser = User::FindUser(nUserID);
	if (pUser == nullptr)
		return nullptr;
	ScriptUser* p = AllocObj(ScriptUser);
	p->m_pUser = pUser;
	return p;
}

void ScriptUser::DestroySelf(lua_State * L, ScriptUser * p)
{
	FreeObj(p);
}

void ScriptUser::Register(lua_State * L)
{
	static luaL_Reg TargetMetatable[] = {
		{ "noticeMsg", TargetNoticeMessage },
		{ "chatMsg", TargetChatMessage },
		{ "inventory", TargetInventory }, 
		{ "questRecord", TargetQuestRecord },
		{ "questEndEffect", TargetQuestEndEffect },
		{ "getField", TargetGetField },
		{ "playPortalSE", TargetPlayPortalSE },
		{ "transferField", TargetRegisterTransferField },
		{ "incHP", TargetIncHP },
		{ "incMP", TargetIncMP },
		{ "incSTR", TargetIncSTR },
		{ "incINT", TargetIncINT },
		{ "incDEX", TargetIncDEX },
		{ "incLUK", TargetIncLUK },
		{ "incSP", TargetIncSP },
		{ "incAP", TargetIncAP },
		{ "incMMP", TargetIncMMP },
		{ "incMHP", TargetIncMHP },
		{ "incEXP", TargetIncEXP },
		{ "incMoney", TargetIncMoney },
		{ "setJob", TargetSetJob },
		{ "getLevel", TargetGetLevel },
		{ "getJob", TargetGetJob },
		{ "getHP", TargetGetHP },
		{ "getMP", TargetGetMP },
		{ "getSTR", TargetGetSTR },
		{ "getINT", TargetGetINT },
		{ "getDEX", TargetGetDEX },
		{ "getLUK", TargetGetLUK },
		{ "getSP", TargetGetSP },
		{ "getAP", TargetGetAP },
		{ "getMMP", TargetGetMMP },
		{ "getMHP", TargetGetMHP },
		{ "getEXP", TargetGetEXP },
		{ "getMoney", TargetGetMoney },
		{ "getHair", TargetGetHair },
		{ "getFace", TargetGetFace },
		{ "getSkin", TargetGetSkin },
		{ "getGender", TargetGetGender },
		{ "isWearing", TargetIsWearing },
		{ "getPosX", TargetGetPosX },
		{ "getPosY", TargetGetPosY },
		{ "giveBuff", TargetGiveBuff },
		{ "cancelBuff", TargetCancelBuff },
		{ "isGuildMember", TargetIsGuildMember },
		{ "isGuildMaster", TargetIsGuildMaster },
		{ "getGuildCountMax", TargetGetGuildCountMax },
		{ "isPartyBoss", TargetIsPartyBoss },
		{ "getPartyMemberJob", TargetGetPartyMemberJob },
		{ "getPartyMemberLevel", TargetGetPartyMemberLevel },
		{ "givePartyBuff", TargetGivePartyBuff },
		{ "cancelPartyBuff", TargetCancelPartyBuff },
		{ "getPartyID", TargetGetPartyID },
		{ "getMCTeam", TargetGetMCTeam },
		{ "getName", TargetGetName },
		{ "createNewGuild", TargetCreateNewGuild },
		{ "removeGuild", TargetRemoveGuild },
		{ "setGuildMark", TargetSetGuildMark },
		{ "removeGuildMark", TargetRemoveGuildMark },
		{ "isGuildMarkExist", TargetIsGuildMarkExist },
		{ "incGuildCountMax", TargetIncGuildCountMax },
		{ "message", TargetMessage },
		{ "exception1", TargetException1 },
		{ "exception2", TargetException2 },
		{ NULL, NULL }
	};

	static luaL_Reg TargetTable[] = {
		{ NULL, NULL }
	};

	luaW_register<ScriptUser>(L, "User", 
		TargetTable, 
		TargetMetatable, 
		&(ScriptUser::GetSelf),
		&(ScriptUser::DestroySelf));
}

int ScriptUser::TargetChatMessage(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nType = (int)luaL_checkinteger(L, 2);
	const char* sMsg = luaL_checkstring(L, 3);
	self->m_pUser->SendChatMessage(nType, sMsg);
	lua_pushinteger(L, 1);
	return 1;
}

int ScriptUser::TargetNoticeMessage(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	//int nType = (int)luaL_checkinteger(L, 2);
	const char* sMsg = luaL_checkstring(L, 2);
	self->m_pUser->SendNoticeMessage(sMsg);
	lua_pushinteger(L, 1);
	return 1;
}

int ScriptUser::TargetInventory(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	ScriptInventory* obj = (self->m_pUser == nullptr ? nullptr : AllocObj(ScriptInventory));
	if (obj)
		obj->SetUser(self->m_pUser);

	((Script*)L->selfPtr)->PushClassObject(obj);
	return 1;
}

int ScriptUser::TargetQuestRecord(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	ScriptQuestRecord* obj = 
		(self->m_pUser == nullptr ? nullptr : AllocObj(ScriptQuestRecord));
	if (obj)
		obj->SetUser(self->m_pUser);

	((Script*)L->selfPtr)->PushClassObject(obj);
	return 1;
}

int ScriptUser::TargetGetField(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	ScriptField* obj =
		(self->m_pUser == nullptr ? nullptr :
			(self->m_pUser->GetField() == nullptr ? nullptr : AllocObj(ScriptField)));
	if (obj)
		obj->SetField(self->m_pUser->GetField());

	((Script*)L->selfPtr)->PushClassObject(obj);
	return 1;
}

int ScriptUser::TargetPlayPortalSE(lua_State *L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	self->m_pUser->SendPlayPortalSE();
	return 1;
}

int ScriptUser::TargetRegisterTransferField(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nFieldID = (int)luaL_checkinteger(L, 2);
	const char *sPortal = luaL_checkstring(L, 3);
	self->GetUser()->TryTransferField(nFieldID, sPortal);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetGetPosX(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	lua_pushinteger(L, self->m_pUser->GetPosX());
	return 1;
}

int ScriptUser::TargetGetPosY(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	lua_pushinteger(L, self->m_pUser->GetPosY());
	return 1;
}

int ScriptUser::TargetIncHP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);
	
	auto liFlag = QWUser::IncHP(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncMP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncMP(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncSTR(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncSTR(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncINT(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncINT(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncDEX(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncDEX(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncLUK(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncLUK(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncSP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);
	int nJobLevel = (int)(lua_gettop(L) > 3 ? luaL_checkinteger(L, 4) : -1);

	auto liFlag = QWUser::IncSP(
		self->m_pUser, 
		(nJobLevel != -1 ? nJobLevel : 
			WvsGameConstants::GetJobLevel(
				self->m_pUser->GetCharacterData()->mStat->nJob
			)
		),
		nVal, 
		nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncAP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncAP(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncMMP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncMMP(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncMHP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncMHP(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncEXP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncEXP(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetIncMoney(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);
	int nFullOnly = (int)(lua_gettop(L) > 2 ? luaL_checkinteger(L, 3) : 0);

	auto liFlag = QWUser::IncMoney(self->m_pUser, nVal, nFullOnly == 1);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetSetJob(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nVal = (int)luaL_checkinteger(L, 2);

	auto liFlag = QWUser::SetJob(self->m_pUser, nVal);
	self->m_pUser->SendCharacterStat(false, liFlag);
	self->m_pUser->ValidateStat();
	return 1;
}

int ScriptUser::TargetGetLevel(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetLevel(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetJob(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetJob(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetHP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetHP(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetMP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetMP(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetSTR(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetSTR(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetINT(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetINT(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetDEX(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetDEX(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetLUK(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetLUK(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetSP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nJobLevel = (int)(lua_gettop(L) > 1 ? luaL_checkinteger(L, 2) : -1);

	lua_pushinteger(L, QWUser::GetSP(
		self->m_pUser,
		nJobLevel != -1 ? 
		nJobLevel : 0
		/*WvsGameConstants::GetJobLevel(
			self->m_pUser->GetCharacterData()->mStat->nJob
		)*/
	));
	return 1;
}

int ScriptUser::TargetGetAP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetAP(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetMMP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetMaxMPVal(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetMHP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetMaxHPVal(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetEXP(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetEXP(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetMoney(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);

	lua_pushinteger(L, QWUser::GetMoney(self->m_pUser));
	return 1;
}

int ScriptUser::TargetGetHair(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	lua_pushinteger(L, self->m_pUser->GetCharacterData()->mStat->nHair);
	return 1;
}

int ScriptUser::TargetGetFace(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	lua_pushinteger(L, self->m_pUser->GetCharacterData()->mStat->nFace);
	return 1;
}

int ScriptUser::TargetGetSkin(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	lua_pushinteger(L, self->m_pUser->GetCharacterData()->mStat->nSkin);
	return 1;
}

int ScriptUser::TargetGetGender(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	lua_pushinteger(L, self->m_pUser->GetCharacterData()->mStat->nGender);
	return 1;
}

int ScriptUser::TargetIsWearing(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nItemID = (int)luaL_checkinteger(L, 2);

	lua_pushinteger(L, self->m_pUser->GetCharacterData()->IsWearing(nItemID));
	return 1;
}

int ScriptUser::TargetQuestEndEffect(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	self->GetUser()->SendQuestEndEffect();
	return 1;
}

int ScriptUser::TargetIsGuildMember(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nGuildID = GuildMan::GetInstance()->GetGuildIDByCharID(self->m_pUser->GetUserID());
	lua_pushinteger(L, nGuildID != -1 ? 1 : 0);
	return 1;
}

int ScriptUser::TargetIsGuildMaster(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nGuildID = GuildMan::GetInstance()->GetGuildIDByCharID(self->m_pUser->GetUserID());
	lua_pushinteger(L, GuildMan::GetInstance()->IsGuildMaster(nGuildID, self->m_pUser->GetUserID()) ? 1 : 0);
	return 1;
}

int ScriptUser::TargetGetGuildCountMax(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	auto pGuild = GuildMan::GetInstance()->GetGuildByCharID(self->m_pUser->GetUserID());
	lua_pushinteger(L, pGuild ? pGuild->nMaxMemberNum : 0);
	return 1;
}

int ScriptUser::TargetIsPartyBoss(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	auto pParty = PartyMan::GetInstance()->GetPartyByCharID(self->m_pUser->GetUserID());
	if(!pParty)
		lua_pushinteger(L, 0);
	else
		lua_pushinteger(L, self->m_pUser->GetUserID() == pParty->party.nPartyBossCharacterID ? 1 : 0);
	return 1;
}

int ScriptUser::TargetGetPartyMemberJob(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nIdx = (int)luaL_checkinteger(L, 2);
	auto pParty = PartyMan::GetInstance()->GetPartyByCharID(self->m_pUser->GetUserID());
	if (!pParty || nIdx < 0 || nIdx >= PartyMan::MAX_PARTY_MEMBER_COUNT)
		lua_pushinteger(L, 0);
	else
		lua_pushinteger(L, pParty->party.anJob[nIdx]);
	return 1;
}

int ScriptUser::TargetGetPartyMemberLevel(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nIdx = (int)luaL_checkinteger(L, 2);
	auto pParty = PartyMan::GetInstance()->GetPartyByCharID(self->m_pUser->GetUserID());
	if (!pParty || nIdx < 0 || nIdx >= PartyMan::MAX_PARTY_MEMBER_COUNT)
		lua_pushinteger(L, 0);
	else
		lua_pushinteger(L, pParty->party.anLevel[nIdx]);
	return 1;
}

int ScriptUser::TargetGiveBuff(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nItemID = (int)luaL_checkinteger(L, 2);
	auto pItem = ItemInfo::GetInstance()->GetStateChangeItem(nItemID);
	if (!pItem)
		return 1;
	pItem->Apply(
		self->m_pUser,
		GameDateTime::GetTime(),
		false
	);
	return 1;
}

int ScriptUser::TargetCancelBuff(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nItemID = (int)luaL_checkinteger(L, 2);
	auto pItem = ItemInfo::GetInstance()->GetStateChangeItem(nItemID);
	if (!pItem)
		return 1;
	pItem->Apply(
		self->m_pUser,
		GameDateTime::GetTime(),
		false,
		true
	);
	return 1;
}

int ScriptUser::TargetGivePartyBuff(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nItemID = (int)luaL_checkinteger(L, 2);
	auto pItem = ItemInfo::GetInstance()->GetStateChangeItem(nItemID);
	if (!pItem)
		return 1;
	pItem->Apply(
		self->m_pUser,
		GameDateTime::GetTime(),
		false
	);
	if (self->m_pUser->GetPartyID() == -1)
		return 1;

	int anCharacterID[PartyMan::MAX_PARTY_MEMBER_COUNT];
	PartyMan::GetInstance()->GetSnapshot(
		self->m_pUser->GetPartyID(),
		anCharacterID
	);
	ZSharedPtr<User> pUser;
	for(auto nID : anCharacterID)
		if(nID != self->m_pUser->GetUserID() &&
			((pUser = User::FindUser(nID)) != nullptr))
			pItem->Apply(
				pUser,
				GameDateTime::GetTime(),
				false
			);
			
	return 1;
}

int ScriptUser::TargetCancelPartyBuff(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nItemID = (int)luaL_checkinteger(L, 2);
	auto pItem = ItemInfo::GetInstance()->GetStateChangeItem(nItemID);
	if (!pItem)
		return 1;

	pItem->Apply(
		self->m_pUser,
		GameDateTime::GetTime(),
		false,
		true
	);
	if (self->m_pUser->GetPartyID() == -1)
		return 1;

	int anCharacterID[PartyMan::MAX_PARTY_MEMBER_COUNT];
	PartyMan::GetInstance()->GetSnapshot(
		self->m_pUser->GetPartyID(),
		anCharacterID
	);

	ZSharedPtr<User> pUser;
	for (auto nID : anCharacterID)
		if (nID != self->m_pUser->GetUserID() &&
			((pUser = User::FindUser(nID)) != nullptr))
			pItem->Apply(
				pUser,
				GameDateTime::GetTime(),
				false,
				true
			);
	return 1;
}

int ScriptUser::TargetGetPartyID(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	auto pParty = PartyMan::GetInstance()->GetPartyByCharID(self->m_pUser->GetUserID());
	if (!pParty)
		lua_pushinteger(L, -1);
	else
		lua_pushinteger(L, pParty->nPartyID);
	return 1;
}

int ScriptUser::TargetGetMCTeam(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	lua_pushinteger(L, self->m_pUser->GetMCarnivalTeam());
	return 1;
}

int ScriptUser::TargetGetName(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	lua_pushstring(L, self->m_pUser->GetName().c_str());
	return 1;
}

int ScriptUser::TargetCreateNewGuild(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	if (QWUser::GetMoney(self->m_pUser) < GuildMan::CREATE_GUILD_COST)
	{
		self->m_pUser->SendNoticeMessage("楓幣不足，需要" + std::to_string(GuildMan::CREATE_GUILD_COST) + "楓幣才可以建立公會。");
		self->m_pUser->SendCharacterStat(true, 0);
		return 0; //Force terminate
	}
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnGuildResult);
	oPacket.Encode1(GuildMan::GuildResult::res_Guild_Create);
	self->m_pUser->SendPacket(&oPacket);
	return 0; //Force terminate
}

int ScriptUser::TargetRemoveGuild(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	if (QWUser::GetMoney(self->m_pUser) < GuildMan::CREATE_GUILD_COST)
	{
		self->m_pUser->SendNoticeMessage("楓幣不足，需要" + std::to_string(GuildMan::REMOVE_GUILD_COST) + "楓幣才可以解散公會。");
		self->m_pUser->SendCharacterStat(true, 0);
		return 0; //Force terminate
	}

	auto pGuild = GuildMan::GetInstance()->GetGuildByCharID(self->m_pUser->GetUserID());
	if (!pGuild || !GuildMan::GetInstance()->IsGuildMaster(pGuild->nGuildID, self->m_pUser->GetUserID()))
	{
		self->m_pUser->SendNoticeMessage("只有會長才可以解散公會。");
		self->m_pUser->SendCharacterStat(true, 0);
		return 0; //Force terminate
	}

	GuildMan::GetInstance()->OnRemoveGuildRequest(self->m_pUser);
	return 0;
}

int ScriptUser::TargetSetGuildMark(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	if (QWUser::GetMoney(self->m_pUser) < GuildMan::CREATE_GUILD_COST)
	{
		self->m_pUser->SendNoticeMessage("楓幣不足，需要" + std::to_string(GuildMan::SET_MARK_COST) + "楓幣才可以設定徽章。");
		self->m_pUser->SendCharacterStat(true, 0);
		return 0; //Force terminate
	}

	auto pGuild = GuildMan::GetInstance()->GetGuildByCharID(self->m_pUser->GetUserID());
	if (!pGuild || !GuildMan::GetInstance()->IsGuildMaster(pGuild->nGuildID, self->m_pUser->GetUserID()))
	{
		self->m_pUser->SendNoticeMessage("只有會長才可以設定徽章。");
		self->m_pUser->SendCharacterStat(true, 0);
		return 0; //Force terminate
	}
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnGuildResult);
	oPacket.Encode1(GuildMan::GuildResult::res_Guild_AskMark);
	self->m_pUser->SendPacket(&oPacket);
	return 0;
}

int ScriptUser::TargetRemoveGuildMark(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	if (QWUser::GetMoney(self->m_pUser) < GuildMan::CREATE_GUILD_COST)
	{
		self->m_pUser->SendNoticeMessage("楓幣不足，需要" + std::to_string(GuildMan::REMOVE_MARK_COST) + "楓幣才可以移除徽章。");
		self->m_pUser->SendCharacterStat(true, 0);
		return 0; //Force terminate
	}

	auto pGuild = GuildMan::GetInstance()->GetGuildByCharID(self->m_pUser->GetUserID());
	if (!pGuild || !GuildMan::GetInstance()->IsGuildMaster(pGuild->nGuildID, self->m_pUser->GetUserID()))
	{
		self->m_pUser->SendNoticeMessage("只有會長才可以移除徽章。");
		self->m_pUser->SendCharacterStat(true, 0);
		return 0; //Force terminate
	}
	unsigned char aMark[] = { 0, 0, 0, 0, 0, 0 };
	InPacket iPacket(aMark, 6);
	GuildMan::GetInstance()->OnSetMarkRequest(self->m_pUser, &iPacket);
	return 0;
}

int ScriptUser::TargetIsGuildMarkExist(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	auto pGuild = GuildMan::GetInstance()->GetGuildByCharID(self->m_pUser->GetUserID());
	lua_pushinteger(L, pGuild ? (pGuild->nMark != 0 ? 1 : 0) : 0);
	return 1;
}

int ScriptUser::TargetIncGuildCountMax(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	int nInc = (int)luaL_checkinteger(L, 2);
	int nCost = (int)luaL_checkinteger(L, 3);
	if (QWUser::GetMoney(self->m_pUser) < nCost)
	{
		self->m_pUser->SendNoticeMessage("楓幣不足，需要" + std::to_string(nCost) + "楓幣才可以增加公會人數。");
		self->m_pUser->SendCharacterStat(true, 0);
		return 0; //Force terminate
	}

	auto pGuild = GuildMan::GetInstance()->GetGuildByCharID(self->m_pUser->GetUserID());
	if (!pGuild || !GuildMan::GetInstance()->IsGuildMaster(pGuild->nGuildID, self->m_pUser->GetUserID()))
	{
		self->m_pUser->SendNoticeMessage("只有會長才可以增加公會人數。");
		self->m_pUser->SendCharacterStat(true, 0);
		return 0; //Force terminate
	}

	GuildMan::GetInstance()->OnIncMaxMemberNumRequest(self->m_pUser, nInc, nCost);
	return 1;
}

int ScriptUser::TargetMessage(lua_State * L)
{
	ScriptUser* self = luaW_check<ScriptUser>(L, 1);
	std::string sMsg = luaL_checkstring(L, 2);
	self->m_pUser->SendChatMessage(5, sMsg);
	return 1;
}

int ScriptUser::TargetException1(lua_State * L)
{
	int *p = nullptr;
	*p = 0;
	return 0;
}

int ScriptUser::TargetException2(lua_State * L)
{
	int *p = new int;
	delete[] p;
	return 0;
}
