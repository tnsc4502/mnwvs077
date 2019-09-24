#include "WzFileSystem.h"
#include "WzArchive.h"
#include "StandardFileSystem.h"
#include "..\Memory\MemoryPoolMan.hpp"
#include "WzNameSpace.h"
#include <iostream>
#include <fstream>
#include "WzStreamCodec.h"

WzFileSystem::WzFileSystem()
{
}

WzFileSystem::~WzFileSystem()
{
	if (m_pChiper)
		FreeObj(m_pChiper);
}

const filesystem::path WzFileSystem::GetAbsPath(const filesystem::path& fPath) const
{
	auto sPath = fPath.wstring();
	if (sPath.find(':') != sPath.npos)
		return sPath;

	auto fsPath = filesystem::current_path();
	fsPath += (sPath);
	return fsPath;
}

WzNameSpace *WzFileSystem::TryMount(const filesystem::path& sArchiveName)
{
	auto pResult = WzArchive::Mount(
		m_sFileSysPath.wstring() + sArchiveName.wstring(),
		sArchiveName.string(),
		m_pChiper
	);

	if (pResult)
		m_mArchive[sArchiveName] = pResult;
	return pResult ? pResult->GetRoot() : nullptr;
}

void WzFileSystem::Init(const filesystem::path &sPath)
{
	m_sFileSysPath = GetAbsPath(sPath);
	m_bInitialized = true;
	m_pChiper = AllocObj(CipherType);
}

WzNameSpace* WzFileSystem::GetItem(const filesystem::path &sArchiveName)
{
	auto r = m_mArchive.find(sArchiveName);
	if (r == m_mArchive.end())
		return TryMount(sArchiveName);
	
	return r->second->GetRoot();
}

void WzFileSystem::Unmount(const filesystem::path& sArchiveName)
{
	auto pItem = GetItem(sArchiveName);
	if (pItem)
		FreeObj(pItem);
	m_mArchive.erase(sArchiveName);
}
