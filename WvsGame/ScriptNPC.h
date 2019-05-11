#pragma once
#include "..\WvsLib\Script\lua.h"

class Script;
class InPacket;

class ScriptNPC
{
	enum ScriptStyle
	{
		eNoESC = 0x1,
		ePlayerRespond = 0x02,
		eInverseDirection = 0x08,
		ePlayerTalk = 0x10,
		eFullScreen = 0x20
	};

	enum ScriptType
	{
		OnSay = 0x00,
		OnAskYesNo = 0x01,
		OnAskText = 0x02,
		OnAskNumber = 0x03,
		OnAskMenu = 0x04,
		OnAskAvatar = 0x07,
		OnInitialQuiz = 0x05,
		OnInitialSpeedQuiz = 0x09,
		OnICQuiz = 0x0A,
		OnAskAcceptDecline = 0x11,
	};
public:
	ScriptNPC();
	~ScriptNPC();

	static ScriptNPC* GetSelf(lua_State* L);
	static void DestroySelf(lua_State* L, ScriptNPC* p);
	static void Register(lua_State* L);
	static void OnPacket(InPacket *iPacket, Script* pScript, lua_State* L);

	static int SelfSay(lua_State* L);
	static int SelfAskAvatar(lua_State* L);
	static int SelfSayNext(lua_State* L);
	static void SelfSayNextImpl(lua_State* L, int nPage);

	static int SelfAskText(lua_State* L);
	static int SelfAskNumber(lua_State* L);
	static int SelfAskYesNo(lua_State* L);
	static int SelfAskAcceptDecline(lua_State* L);
	static int SelfAskMenu(lua_State* L);
	static int SelfGetField(lua_State* L);
	static int Debug(lua_State* L);

	static void CheckMessageParameter(lua_State* L, int nStartIndex, void *pInfo);
	static void MakeMessagePacket(lua_State* L, void *pInfo);
};

