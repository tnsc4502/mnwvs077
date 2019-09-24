#include "WzNameSpaceProperty.h"
#include "WzArchive.h"
#include "WzProperty.h"
#include "..\Memory\MemoryPoolMan.hpp"

#include <iostream>

//This actually is "ClipArchive" impl. in official codes.
void WzNameSpaceProperty::ClipArchive()
{
	m_pArchive->SetPosition(m_uBeginPos);
	m_pProperty = AllocObjCtor(WzProperty)(m_pArchive, GetName(), m_uBeginPos, m_uBeginPos);
	m_bParsed = true;
}

WzNameSpaceProperty::WzNameSpaceProperty(WzArchive *pArchive, WzNameSpaceType nNameSpaceType, const std::string& sName, unsigned int uBeginPos)
	: WzNameSpace(pArchive, nNameSpaceType, sName, uBeginPos)
{
}

WzNameSpaceProperty::~WzNameSpaceProperty()
{
	if (m_pProperty)
		FreeObj(m_pProperty);
}

WzProperty* WzNameSpaceProperty::GetProperty()
{
	return m_pProperty;
}

void WzNameSpaceProperty::OnGetItem()
{
	if (!m_bParsed)
		ClipArchive();
}
