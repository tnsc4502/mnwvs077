#pragma once
#include <string>
#include <map>

class WzArchive;
class WzProperty;
class WzNameSpace
{
	friend class WzIterator;
public:
	typedef std::map<std::string, WzNameSpace*> NameSpaceContainerType;

	enum WzNameSpaceType
	{
		Type_Directory = 3,
		Type_Property = 4,
	};

protected:
	NameSpaceContainerType m_mChild;
	WzArchive* m_pArchive = nullptr;
	WzNameSpaceType m_nNameSpaceType;
	unsigned int m_uBeginPos = 0;
	std::string m_sName;
	bool m_bParsed = false;

	WzNameSpace(WzArchive *pArchive, WzNameSpaceType nNameSpaceType, const std::string& sName, unsigned int uBeginPos);

public:
	WzNameSpace* GetItem(const std::string& sName);
	virtual void OnGetItem() = 0;
	virtual WzProperty* GetProperty();
	const std::string& GetName() const;
	WzNameSpaceType GetNameSpaceType() const;
	virtual ~WzNameSpace() = 0;
};

