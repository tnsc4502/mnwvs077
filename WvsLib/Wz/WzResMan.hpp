#pragma once

#include "WzFileSystem.h"
#include "WzIterator.h"
#include "..\Exception\WvsException.h"
#include "..\Memory\MemoryPoolMan.hpp"
#include "..\Common\ConfigLoader.hpp"
#include "..\Common\ServerConstants.hpp"

#define MAPPING_WZ_NAME(name, path) m_aWzNode[(int)Wz::name] = m_FileSystem.GetItem(#name)

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

class WzResMan
{
private:
	WzFileSystem m_FileSystem;
	WzNameSpace* m_aWzNode[(int)Wz::UI + 1];
	ConfigLoader* pCfg = nullptr;

	void Init()
	{
		m_aWzNode[(int)Wz::Base] = m_FileSystem.GetItem("./Base.wz");
		m_aWzNode[(int)Wz::Character] = m_FileSystem.GetItem("./Character.wz");
		m_aWzNode[(int)Wz::Effect] = m_FileSystem.GetItem("./Effect.wz");
		m_aWzNode[(int)Wz::Etc] = m_FileSystem.GetItem("./Etc.wz");
		m_aWzNode[(int)Wz::Item] = m_FileSystem.GetItem("./Item.wz");
		m_aWzNode[(int)Wz::Map] = m_FileSystem.GetItem("./Map.wz");
		m_aWzNode[(int)Wz::Map2] = m_FileSystem.GetItem("./Map2.wz");
		m_aWzNode[(int)Wz::Mob] = m_FileSystem.GetItem("./Mob.wz");
		m_aWzNode[(int)Wz::Mob2] = m_FileSystem.GetItem("./Mob2.wz");
		m_aWzNode[(int)Wz::Morph] = m_FileSystem.GetItem("./Morph.wz");
		m_aWzNode[(int)Wz::Npc] = m_FileSystem.GetItem("./Npc.wz");
		m_aWzNode[(int)Wz::Quest] = m_FileSystem.GetItem("./Quest.wz");
		m_aWzNode[(int)Wz::Reactor] = m_FileSystem.GetItem("./Reactor.wz");
		m_aWzNode[(int)Wz::Skill] = m_FileSystem.GetItem("./Skill.wz");
		m_aWzNode[(int)Wz::String] = m_FileSystem.GetItem("./String.wz");
		m_aWzNode[(int)Wz::TamingMob] = m_FileSystem.GetItem("./TamingMob.wz");
		m_aWzNode[(int)Wz::UI] = m_FileSystem.GetItem("./UI.wz");
	}

public:

	static WzResMan* GetInstance()
	{
		static WzResMan *sWzResMan = new WzResMan();
		return sWzResMan;
	}

	void Init(const std::string& sGlobalConfigPath)
	{
		pCfg = ConfigLoader::Get(sGlobalConfigPath);
		if (!pCfg || pCfg->StrValue("DataDir") == "")
			WvsException::FatalError("[WvsLib -- WzResMan::Init]Unable to find the global config file (the path is specified as GlobalConfig in the application config) which defines the DataDir value.");

		m_FileSystem.Init(pCfg->StrValue("DataDir"));
		Init();
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

	void RemountAll();
};