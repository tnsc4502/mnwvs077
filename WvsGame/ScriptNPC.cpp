#include "ScriptNPC.h"
#include "ScriptField.h"
#include "Script.h"
#include "User.h"
#include "QWUser.h"
#include "QWUInventory.h"
#include "WvsGame.h"
#include "GuildMan.h"
#include "NpcPacketTypes.hpp"
#include "UserPacketTypes.hpp"

#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\WvsCenter\CenterPacketTypes.hpp"
#include "..\WvsLib\Script\luaconf.h"
#include "..\WvsLib\Script\lvm.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Random\Rand32.h"

ScriptNPC::ScriptNPC()
{
}


ScriptNPC::~ScriptNPC()
{
}

ScriptNPC * ScriptNPC::GetSelf(lua_State * L)
{
	auto pSys =((Script*)L->selfPtr);
	if (pSys->GetID() == 0)
		return nullptr;

	if (pSys->m_pUniqueScriptBinding == nullptr)
		pSys->m_pUniqueScriptBinding = AllocObj(ScriptNPC);
	return (ScriptNPC*)pSys->m_pUniqueScriptBinding;
}

void ScriptNPC::DestroySelf(lua_State * L, ScriptNPC * p)
{
	FreeObj(p);
}

void ScriptNPC::Register(lua_State * L)
{
	static luaL_Reg SelfMetatable[] = {
		{ "askAvatar", SelfAskAvatar },
		{ "makeRandAvatar", SelfMakeRandAvatar },
		{ "askText", SelfAskText },
		{ "askYesNo", SelfAskYesNo },
		{ "askAccept", SelfAskAcceptDecline },
		{ "askNumber", SelfAskNumber },
		{ "askMenu", SelfAskMenu },
		{ "sayNext", SelfSayNext },
		{ "say", SelfSay },
		{ "getField", SelfGetField },
		{ "dealWithGuildQuest", SelfDealWithGuildQuest },
		{ "debug", Debug },
		{ NULL, NULL }
	};

	static luaL_Reg SelfTable[] = {
		{ NULL, NULL }
	};
	lua_pushinteger(L, ScriptStyle::ePlayerTalk);
	lua_setglobal(L, "style_playerTalk");
	lua_pushinteger(L, ScriptStyle::ePlayerRespond);
	lua_setglobal(L, "style_playerRespond");
	lua_pushinteger(L, ScriptStyle::eFullScreen);
	lua_setglobal(L, "style_fullScreen");
	lua_pushinteger(L, ScriptStyle::eInverseDirection);
	lua_setglobal(L, "style_inverseDirection");
	lua_pushinteger(L, ScriptStyle::eNoESC);
	lua_setglobal(L, "style_noESC");

	luaW_register<ScriptNPC>(L,
		"Npc", 
		SelfTable, 
		SelfMetatable, 
		&(ScriptNPC::GetSelf),
		&(ScriptNPC::DestroySelf));
}

void ScriptNPC::OnPacket(InPacket * iPacket, Script * pScript, lua_State* L)
{
	char nMsgType = iPacket->Decode1(), nAction = 0;
	if (nMsgType != ScriptMessageType::OnSay || pScript->GetConverstaionState().m_bPaging == false) 
	{
		pScript->GetConverstaionState().m_aPageStack.clear();
		pScript->GetConverstaionState().m_nCurPage = 0;
	}

	switch (nMsgType)
	{
		case ScriptMessageType::OnSay:
		{
			nAction = iPacket->Decode1();
			if (nAction == (char)0xFF) 
			{
				pScript->Abort();
				return;
			}
			else if (nAction == 0) 
			{
				pScript->GetConverstaionState().m_nUserInput = 0;
				if (pScript->GetConverstaionState().m_bPaging 
					&& pScript->GetConverstaionState().m_nCurPage != 1)
				{
					SelfSayNextImpl(L, --(pScript->GetConverstaionState().m_nCurPage) - 1);
					return;
				}
			}
			else if (nAction == 1) 
			{
				pScript->GetConverstaionState().m_nUserInput = 1;
				if (pScript->GetConverstaionState().m_bPaging
					&& pScript->GetConverstaionState().m_nCurPage != pScript->GetConverstaionState().m_aPageStack.size())
				{
					SelfSayNextImpl(L, ++(pScript->GetConverstaionState().m_nCurPage) - 1);
					return;
				}
			}
			break;
		}
		case ScriptMessageType::OnAskAcceptDecline:
		case ScriptMessageType::OnAskYesNo:
		{
			nAction = iPacket->Decode1();
			if (nAction == (char)0xFF) 
			{
				pScript->Abort();
				return;
			}
			else if (nAction == 0)
				pScript->GetConverstaionState().m_nUserInput = 0;
			else if (nAction == 1)
				pScript->GetConverstaionState().m_nUserInput = 1;
			lua_pushinteger(L, pScript->GetConverstaionState().m_nUserInput);
			pScript->GetConverstaionState().m_bResume = true;
			break;
		}
		case ScriptMessageType::OnAskText:
		{
			nAction = iPacket->Decode1();
			if (nAction == 0) 
			{
				pScript->Abort();
				return;
			}
			else if (nAction == 1)
				lua_pushstring(L, iPacket->DecodeStr().c_str());
			pScript->GetConverstaionState().m_bResume = true;
			break;
		}
		case ScriptMessageType::OnAskNumber:
		{
			nAction = iPacket->Decode1();
			if (nAction == 0) 
			{
				pScript->Abort();
				return;
			}
			else if (nAction == 1)
				pScript->GetConverstaionState().m_nUserInput = iPacket->Decode4();
			lua_pushinteger(L, pScript->GetConverstaionState().m_nUserInput);
			pScript->GetConverstaionState().m_bResume = true;
			break;
		}
		case ScriptMessageType::OnAskMenu:
		{
			nAction = iPacket->Decode1();
			if (nAction == 0) 
			{
				pScript->Abort();
				return;
			}
			else if (nAction == 1)
				pScript->GetConverstaionState().m_nUserInput = iPacket->Decode4();
			lua_pushinteger(L, pScript->GetConverstaionState().m_nUserInput);
			pScript->GetConverstaionState().m_bResume = true;
			break;
		}
		case ScriptMessageType::OnAskAvatar:
		{
			int nResult = 1;
			nAction = iPacket->Decode1();
			if (nAction == 0) 
			{
				pScript->Abort();
				return;
			}
			else if (nAction == 1)
			{
				auto& refInfo = pScript->GetLastConversationInfo();
				pScript->GetConverstaionState().m_nUserInput = iPacket->Decode1();
				if (pScript->GetConverstaionState().m_nUserInput < refInfo.m_aIntObj.size())
				{
					std::vector<ExchangeElement> aExchange;
					aExchange.push_back({});
					aExchange.back().m_nItemID = refInfo.m_nAvatarTicket;
					aExchange.back().m_nCount = -1;
					//leave refInfo.m_nAvatarTicket = 0 for GMs and events
					if (refInfo.m_nAvatarTicket && QWUInventory::Exchange(pScript->GetUser(), 0, aExchange))
						nResult = 0;
					else 
						SetAvatar(pScript->GetUser(), refInfo.m_aIntObj[pScript->GetConverstaionState().m_nUserInput]);
				}	
				refInfo.m_aIntObj.clear();
			}
			lua_pushinteger(L, nResult);
			pScript->GetConverstaionState().m_bResume = true;
			break;
		}
	}
	pScript->GetConverstaionState().m_bPaging = false;
	pScript->Notify();
}

int ScriptNPC::SelfSay(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);

	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptMessageType::OnSay;
	info.m_sTalkText = text;
	info.m_nSpeakerTemplateID = self->GetID();
	//====================

	CheckMessageParameter(L, 3, &info);
	MakeMessagePacket(L, &info);
	self->Wait();
	return 1;
}

void ScriptNPC::SetAvatar(User * pUser, int nSet)
{
	long long liFlag = 0;
	switch (nSet / 10000)
	{
		case 0:
		case 1:
			liFlag |= QWUser::SetSkin(pUser, nSet);
			break;
		case 2:
			liFlag |= QWUser::SetFace(pUser, nSet);
			break;
		case 3:
		case 4:
			liFlag |= QWUser::SetHair(pUser, nSet);
			break;
	}
	pUser->SendCharacterStat(false, liFlag);
	pUser->OnAvatarModified();
}

int ScriptNPC::SelfAskAvatar(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);

	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptMessageType::OnAskAvatar;
	info.m_sTalkText = text;
	info.m_nSpeakerTemplateID = self->GetID();
	//====================

	info.m_nAvatarTicket = (int)luaL_checkinteger(L, 3);
	int nArg = lua_gettop(L);

	if (lua_istable(L, 4))
		self->RetrieveArray(info.m_aIntObj, 4);
	else
	{
		//To be more similar to official style, use varadic args here.
		for (int i = 4; i <= nArg - 1; ++i)
			info.m_aIntObj.push_back((int)luaL_checkinteger(L, i));
	}
	MakeMessagePacket(L, &info);
	self->Wait();
	return 1;
}

int ScriptNPC::SelfMakeRandAvatar(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	Script::NPCConverstaionInfo info;
	info.m_nAvatarTicket = (int)luaL_checkinteger(L, 2);

	int nArg = lua_gettop(L), nResult = 1;

	if (lua_istable(L, 3))
		self->RetrieveArray(info.m_aIntObj, 3);
	else
		for (int i = 3; i <= nArg - 1; ++i)
			info.m_aIntObj.push_back((int)luaL_checkinteger(L, i));
	
	std::vector<ExchangeElement> aExchange;
	aExchange.push_back({});
	aExchange.back().m_nItemID = info.m_nAvatarTicket;
	aExchange.back().m_nCount = -1;

	//leave refInfo.m_nAvatarTicket = 0 for GMs and events
	if (info.m_nAvatarTicket && QWUInventory::Exchange(self->GetUser(), 0, aExchange))
		nResult = 0;
	else
		SetAvatar(self->GetUser(), info.m_aIntObj[(int)Rand32::GetInstance()->Random() % info.m_aIntObj.size()]);
	lua_pushinteger(L, nResult);
	return 1;
}

int ScriptNPC::SelfAskText(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);

	const char* text = luaL_checkstring(L, 2);
	const char * defaultText = luaL_checkstring(L, 3);
	int nMinValue = (int)luaL_checkinteger(L, 4);
	int nMaxValue = (int)luaL_checkinteger(L, 5);

	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptMessageType::OnAskText;
	info.m_sTalkText = text;
	info.m_aStrObj.push_back(defaultText);
	info.m_nSpeakerTemplateID = self->GetID();
	info.m_aIntObj.push_back(nMinValue);
	info.m_aIntObj.push_back(nMaxValue);
	//====================

	MakeMessagePacket(L, &info);
	self->Wait();
	return 1;
}

int ScriptNPC::SelfAskNumber(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);
	int nDefaultValue = (int)luaL_checkinteger(L, 3);
	int nMinValue = (int)luaL_checkinteger(L, 4);
	int nMaxValue = (int)luaL_checkinteger(L, 5);

	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptMessageType::OnAskNumber;
	info.m_sTalkText = text;
	info.m_nSpeakerTemplateID = self->GetID();
	info.m_aIntObj.push_back(nDefaultValue);
	info.m_aIntObj.push_back(nMinValue);
	info.m_aIntObj.push_back(nMaxValue);
	//====================

	MakeMessagePacket(L, &info);
	self->Wait();
	return 1;
}

int ScriptNPC::SelfAskYesNo(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);

	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptMessageType::OnAskYesNo;
	info.m_sTalkText = text;
	info.m_nSpeakerTemplateID = self->GetID();
	//====================

	CheckMessageParameter(L, 3, &info);
	MakeMessagePacket(L, &info);
	self->Wait();
	return 1;
}

int ScriptNPC::SelfAskAcceptDecline(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);

	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptMessageType::OnAskAcceptDecline;
	info.m_sTalkText = text;
	info.m_nSpeakerTemplateID = self->GetID();
	//====================

	CheckMessageParameter(L, 3, &info);
	MakeMessagePacket(L, &info);
	self->Wait();
	return 1;
}

int ScriptNPC::SelfAskMenu(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);
	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptMessageType::OnAskMenu;
	info.m_sTalkText = text;
	info.m_nSpeakerTemplateID = self->GetID();
	//====================

	CheckMessageParameter(L, 3, &info);
	MakeMessagePacket(L, &info);
	self->Wait();
	return 1;
}

int ScriptNPC::SelfGetField(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	ScriptField* obj =
		(self->GetField() == nullptr ? nullptr : AllocObj(ScriptField));
	if (obj)
		obj->SetField(self->GetField());

	((Script*)L->selfPtr)->PushClassObject(obj);
	return 1;
}

int ScriptNPC::SelfDealWithGuildQuest(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	int nType = (int)luaL_checkinteger(L, 2);
	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::WorldQueryRequest);
	oPacket.Encode4(self->GetUser()->GetSocketID());
	oPacket.Encode4(self->GetUser()->GetUserID());
	oPacket.Encode1(CenterWorldQueryType::eWorldQuery_QueryGuildQuest);
	oPacket.Encode1(nType);
	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	self->Wait();
	return 1;
}

int ScriptNPC::Debug(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);
	WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "[Script Debug]%s\n", text);
	return 1;
}

void ScriptNPC::CheckMessageParameter(lua_State * L, int nStartIndex, void * pInfo_)
{
	auto pInfo = (Script::NPCConverstaionInfo*)pInfo_;
	int nArgs = lua_gettop(L), nValue;
	for (int i = nStartIndex; i <= nArgs; ++i)
	{
		nValue = (int)lua_tointeger(L, i);
		pInfo->m_nParam |= nValue;
		pInfo->m_nSpeakerTypeID = 3;
		pInfo->m_nSpeakerTemplateID_ = pInfo->m_nSpeakerTemplateID;
	}
}

void ScriptNPC::MakeMessagePacket(lua_State * L, void * pInfo_)
{
	Script *self = (Script*)L->selfPtr;
	auto pInfo = (Script::NPCConverstaionInfo*)pInfo_;
	OutPacket oPacket;
	oPacket.Encode2((short)NPCSendPacketTypes::NPC_ScriptMessage);
	oPacket.Encode1(pInfo->m_nSpeakerTypeID);
	oPacket.Encode4(pInfo->m_nSpeakerTemplateID);
	oPacket.Encode1(pInfo->m_nMsgType);
	switch (pInfo->m_nMsgType)
	{
		case ScriptMessageType::OnSay:
		{
			if (pInfo->m_nParam & 4)
				oPacket.Encode4(pInfo->m_nSpeakerTemplateID_);
			oPacket.EncodeStr(pInfo->m_sTalkText);
			oPacket.Encode1(pInfo->m_nPage != 0 ? 1 : 0);
			oPacket.Encode1(self->GetConverstaionState().m_bPaging);
			oPacket.Encode4(pInfo->m_tWait);
			break;
		}
		case ScriptMessageType::OnAskMenu:
		case ScriptMessageType::OnAskYesNo:
		case ScriptMessageType::OnAskAcceptDecline:
		{
			if (pInfo->m_nParam & 4)
				oPacket.Encode4(pInfo->m_nSpeakerTemplateID_);
			oPacket.EncodeStr(pInfo->m_sTalkText);
			break;
		}
		case ScriptMessageType::OnAskText:
		case ScriptMessageType::OnAskNumber:
		{
			if (pInfo->m_nMsgType == ScriptMessageType::OnAskText && (pInfo->m_nParam & 4))
				oPacket.Encode4(pInfo->m_nSpeakerTemplateID_);
			oPacket.EncodeStr(pInfo->m_sTalkText);
			if (pInfo->m_nMsgType == ScriptMessageType::OnAskText)
				oPacket.EncodeStr(pInfo->m_aStrObj[0]);
			else
				oPacket.Encode4(pInfo->m_aIntObj[0]);
			if (pInfo->m_nMsgType == ScriptMessageType::OnAskText)
			{
				oPacket.Encode2(pInfo->m_aIntObj[0]);
				oPacket.Encode2(pInfo->m_aIntObj[1]);
			}
			else
			{
				oPacket.Encode4(pInfo->m_aIntObj[1]);
				oPacket.Encode4(pInfo->m_aIntObj[2]);
			}
			break;
		}
		case ScriptMessageType::OnInitialQuiz:
		{
			oPacket.Encode1(0);
			oPacket.EncodeStr(pInfo->m_sTalkText);
			oPacket.EncodeStr(pInfo->m_aStrObj[0]);
			oPacket.EncodeStr(pInfo->m_aStrObj[1]);
			oPacket.Encode4(pInfo->m_aIntObj[0]);
			oPacket.Encode4(pInfo->m_aIntObj[1]);
			oPacket.Encode4(pInfo->m_tWait);
			break;
		}
		case ScriptMessageType::OnInitialSpeedQuiz:
		case ScriptMessageType::OnICQuiz:
		case ScriptMessageType::OnAskAvatar:
		{
			oPacket.EncodeStr(pInfo->m_sTalkText);
			oPacket.Encode1((unsigned char)pInfo->m_aIntObj.size());
			for (const auto& value : pInfo->m_aIntObj)
				oPacket.Encode4(value);
			break;
		}
	}
	if (!self->IsDone() && self->GetUser())
	{
		self->GetUser()->SendPacket(&oPacket);
		self->SetLastConversationInfo(*pInfo);
	}
}

int ScriptNPC::SelfSayNext(lua_State * L)
{
	Script* self = (Script*)(L->selfPtr);
	const char* text = luaL_checkstring(L, 2);
	
	//====================
	Script::NPCConverstaionInfo info;
	info.m_nMsgType = ScriptMessageType::OnSay;
	info.m_sTalkText = text;
	info.m_nSpeakerTemplateID = self->GetID();
	info.m_nPage = self->GetConverstaionState().m_nCurPage;
	//====================

	self->GetConverstaionState().m_aPageStack.push_back(std::move(info));
	self->GetConverstaionState().m_bPaging = true;
	SelfSayNextImpl(L, self->GetConverstaionState().m_nCurPage++);
	self->Wait();
	return 1;
}

void ScriptNPC::SelfSayNextImpl(lua_State * L, int nPage)
{
	Script *self = (Script*)L->selfPtr;
	auto& refStack = self->GetConverstaionState().m_aPageStack;
	if (nPage >= refStack.size())
		return;
	auto& refInfo = refStack[nPage];

	CheckMessageParameter(L, 3, &refInfo);
	MakeMessagePacket(L, &refInfo);
}
