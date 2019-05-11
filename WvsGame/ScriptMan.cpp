#include "ScriptMan.h"
#include "ScriptNPC.h"
#include "ScriptInventory.h"
#include "ScriptFieldSet.h"
#include "ScriptUser.h"
#include "ScriptPacket.h"
#include "ScriptQuestRecord.h"
#include "ScriptField.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

ScriptMan * ScriptMan::GetInstance()
{
	static ScriptMan* pInstance = new ScriptMan;
	return pInstance;
}

Script * ScriptMan::GetScript(const std::string & file, int nTemplateID, Field *pField)
{
	auto pScript = AllocObjCtor(Script)(
		file, 
		nTemplateID,
		pField,
		std::vector<void(*)(lua_State*)>({
		&ScriptNPC::Register,
		&ScriptInventory::Register,
		&ScriptFieldSet::Register,
		&ScriptUser::Register,
		&ScriptQuestRecord::Register,
		&ScriptField::Register,
		&ScriptPacket::Register
	}));
	if (pScript && pScript->Init())
	{
		pScript->m_pOnPacketInvoker = &(ScriptNPC::OnPacket);
		luaL_openlibs(pScript->L);
		return pScript;
	}
	else if (pScript)
		FreeObj(pScript);
	return nullptr;
}
