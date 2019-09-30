#pragma once
#include <string>
#include <map>
#include "..\Common\CommonDef.h"
#include "WzNameSpace.h"
#include "WzDelayedVariant.h"

class WzArchive;

class WzProperty : public WzNameSpace
{
	ALLOW_PRIVATE_ALLOC
public:
	typedef std::map<std::string, WzProperty*> PropContainerType;

private:
	WzDelayedVariant m_wzVariant;

	//Variant property
	WzProperty(const std::string& sPropName);
public:
	WzProperty(WzArchive *pArchive, const std::string& sPropName, unsigned int uBeginPos, unsigned int uRootPropPos);
	~WzProperty();

	void OnGetItem();
	WzDelayedVariant& GetVariant();
	WzProperty* GetProperty();
};

