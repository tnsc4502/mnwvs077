#pragma once
#include "WzNameSpace.h"

class WzProperty;

class WzNameSpaceProperty : public WzNameSpace
{
	WzProperty *m_pProperty = nullptr;

public:
	WzNameSpaceProperty(WzArchive *pArchive, WzNameSpaceType nNameSpaceType, const std::string& sName, unsigned int uBeginPos);
	~WzNameSpaceProperty();
	WzProperty* GetProperty();
	void OnGetItem();
	void ClipArchive();
};

