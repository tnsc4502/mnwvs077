#pragma once
#include "WzNameSpace.h"
#include "WzProperty.h"
#include <vector>

class WzIterator
{
	WzNameSpace *m_pIterNS;

	WzNameSpace::NameSpaceContainerType::iterator m_itNS, m_itEndNS;

	WzIterator();
	WzIterator(WzNameSpace *pNextNS, WzNameSpace::NameSpaceContainerType::iterator& it, WzNameSpace::NameSpaceContainerType::iterator& itEnd);

public:
	WzIterator(WzNameSpace *pTopProperty);
	~WzIterator();

	WzIterator operator[](const std::string& sName);
	WzIterator operator[](const char* sName);
	WzIterator begin();
	const static WzIterator& end();
	WzIterator& operator++();
	WzIterator& operator*();
	const std::string& GetName();
	bool operator != (const WzIterator& rhs);
	bool operator == (const WzIterator& rhs);

	//To support current Wvs projs.
	operator int();
	operator unsigned long long();
	operator double();
	operator float();
	operator const std::string&();

	//For lazy parsing.
	std::vector<std::string> EnumerateChildName();
};

