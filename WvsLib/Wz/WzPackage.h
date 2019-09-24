#pragma once
#include <fstream>
#include "WzNameSpace.h"

class WzPackage : public WzNameSpace
{
private:
	unsigned int LoadPos(unsigned int uBegin, unsigned int uKey);

public:
	WzPackage(WzArchive *pArchive, WzNameSpaceType nNameSpaceType, const std::string& sName, unsigned int uBeginPos);
	~WzPackage();

	WzNameSpace* GetItem(const std::string& sName);
	void OnGetItem();
	bool LoadDirectory(bool bTestKey = false);
	void LoadSubItem();
};

