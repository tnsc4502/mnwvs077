#include "WzNameSpace.h"
#include "..\Memory\MemoryPoolMan.hpp"

WzNameSpace::WzNameSpace(WzArchive *pArchive, WzNameSpaceType nNameSpaceType, const std::string& sName, unsigned int uBeginPos)
{
	m_uBeginPos = uBeginPos;
	m_pArchive = pArchive;
	m_nNameSpaceType = nNameSpaceType;
	m_sName = sName;
}

WzNameSpace* WzNameSpace::GetItem(const std::string & sName)
{
	auto findIter = m_mChild.find(sName);
	if (findIter == m_mChild.end())
		return nullptr;
	auto pResult = findIter->second;
	pResult->OnGetItem();
	return pResult;
}

WzProperty* WzNameSpace::GetProperty()
{
	return nullptr;
}

const std::string& WzNameSpace::GetName() const
{
	return m_sName;
}

WzNameSpace::WzNameSpaceType WzNameSpace::GetNameSpaceType() const
{
	return m_nNameSpaceType;
}

WzNameSpace::~WzNameSpace()
{
	for (auto& prChild : m_mChild)
		FreeObj(prChild.second);
}