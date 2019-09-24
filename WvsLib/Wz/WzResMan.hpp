#pragma once

#include "WzFileSystem.h"
#include "WzIterator.h"
#include "..\Memory\MemoryPoolMan.hpp"
#include "..\Common\ConfigLoader.hpp"
#include "..\Common\ServerConstants.hpp"

#define MAPPING_WZ_NAME(name, path) m_aWzNode[(int)Wz::name] = m_FileSystem.GetItem(#name)

/*
Wz檔案之Tag

By Wanger.
*/
enum class Wz
{
	Base,
	Character,
	Effect,
	Etc,
	Item,
	Map,
	Map2,
	Mob,
	Mob2,
	Morph,
	Npc,
	Quest,
	Reactor,
	Skill,
	String,
	TamingMob,
	UI,
};

/*
Wz資源存取物件

By Wanger.
*/
class WzResMan
{
private:
	WzFileSystem m_FileSystem;
	WzNameSpace* m_aWzNode[(int)Wz::UI + 1];

	WzResMan() 
	{
		auto pCfg = ConfigLoader::Get("GlobalSetting.txt");
		m_FileSystem.Init(pCfg->StrValue("DataDir"));
		
		Init();
	}

	void Init()
	{
		m_aWzNode[(int)Wz::Base] = m_FileSystem.GetItem("./Game/Base.wz");
		m_aWzNode[(int)Wz::Character] = m_FileSystem.GetItem("./Game/Character.wz");
		m_aWzNode[(int)Wz::Effect] = m_FileSystem.GetItem("./Game/Effect.wz");
		m_aWzNode[(int)Wz::Etc] = m_FileSystem.GetItem("./Game/Etc.wz");
		m_aWzNode[(int)Wz::Item] = m_FileSystem.GetItem("./Game/Item.wz");
		m_aWzNode[(int)Wz::Map] = m_FileSystem.GetItem("./Game/Map.wz");
		m_aWzNode[(int)Wz::Map2] = m_FileSystem.GetItem("./Game/Map2.wz");
		m_aWzNode[(int)Wz::Mob] = m_FileSystem.GetItem("./Game/Mob.wz");
		m_aWzNode[(int)Wz::Mob2] = m_FileSystem.GetItem("./Game/Mob2.wz");
		m_aWzNode[(int)Wz::Morph] = m_FileSystem.GetItem("./Game/Morph.wz");
		m_aWzNode[(int)Wz::Npc] = m_FileSystem.GetItem("./Game/Npc.wz");
		m_aWzNode[(int)Wz::Quest] = m_FileSystem.GetItem("./Game/Quest.wz");
		m_aWzNode[(int)Wz::Reactor] = m_FileSystem.GetItem("./Game/Reactor.wz");
		m_aWzNode[(int)Wz::Skill] = m_FileSystem.GetItem("./Game/Skill.wz");
		m_aWzNode[(int)Wz::String] = m_FileSystem.GetItem("./Game/String.wz");
		m_aWzNode[(int)Wz::TamingMob] = m_FileSystem.GetItem("./Game/TamingMob.wz");
		m_aWzNode[(int)Wz::UI] = m_FileSystem.GetItem("./Game/UI.wz");
	}

public:

	static WzResMan* GetInstance()
	{
		static WzResMan *sWzResMan = new WzResMan();
		return sWzResMan;
	}

	WzIterator GetWz(Wz wzTag)
	{
		return WzIterator(m_aWzNode[(int)wzTag]);
	}

	WzIterator GetItem(const std::string& sArchiveName)
	{
		return WzIterator(m_FileSystem.GetItem(sArchiveName));
	}

	void Unmount(const std::string& sArchiveName)
	{
		m_FileSystem.Unmount(sArchiveName);
	}

	void ReleaseMemory()
	{
		//WzMemoryPoolMan::GetInstance()->Release();
		//auto pCfg = ConfigLoader::Get("GlobalSetting.txt");
		//WZ::AddPath(pCfg->StrValue("DataDir"));
		//Init();
	}
};

extern WzResMan *stWzResMan;