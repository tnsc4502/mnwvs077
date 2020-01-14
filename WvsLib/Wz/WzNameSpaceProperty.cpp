#include "WzNameSpaceProperty.h"
#include "WzArchive.h"
#include "WzProperty.h"
#include "..\Memory\MemoryPoolMan.hpp"

#include <iostream>

//This actually is "ClipArchive" impl. in official codes.
void WzNameSpaceProperty::ClipArchive()
{
	m_pProperty = new (WzProperty)(m_pArchive, GetName(), m_uBeginPos, m_uBeginPos);
	m_pProperty->OnGetItem();
	m_bParsed = true;
}

WzNameSpaceProperty::WzNameSpaceProperty(WzArchive *pArchive, WzNameSpaceType nNameSpaceType, const std::string& sName, unsigned int uBeginPos)
	: WzNameSpace(pArchive, nNameSpaceType, sName, uBeginPos)
{
}

WzNameSpaceProperty::~WzNameSpaceProperty()
{
	if (m_pProperty)
		delete (m_pProperty);
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
