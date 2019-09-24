#pragma once
#include "WzNameSpace.h"
#include "WzProperty.h"

class WzIterator
{
	WzProperty *m_pIterProp;
	WzNameSpace *m_pIterNS;

	WzProperty::PropContainerType::iterator m_itProp, m_itEndProp;
	WzNameSpace::NameSpaceContainerType::iterator m_itNS, m_itEndNS;

	WzIterator();
	WzIterator(WzProperty *pNextProp, WzProperty::PropContainerType::iterator& it, WzProperty::PropContainerType::iterator& itEnd);
	WzIterator(WzNameSpace *pNextNS, WzNameSpace::NameSpaceContainerType::iterator& it, WzNameSpace::NameSpaceContainerType::iterator& itEnd);

public:
	WzIterator(WzNameSpace *pTopProperty);
	~WzIterator();

	WzIterator operator[](const std::string& sName);
	WzIterator operator[](const char* sName);
	WzIterator begin();
	WzIterator end();
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
};

