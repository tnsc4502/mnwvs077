#include "WzPackage.h"
#include "WzArchive.h"
#include "..\String\StringUtility.h"
#include "..\Exception\WvsException.h"
#include "WzStreamCodec.h"
#include "..\Memory\MemoryPoolMan.hpp"
#include "WzNameSpaceProperty.h"

#include <iostream>

WzPackage::WzPackage(WzArchive *pArchive, WzNameSpaceType nNameSpaceType, const std::string& sName, unsigned int uBeginPos)
	: WzNameSpace(pArchive, nNameSpaceType, sName, uBeginPos)
{
}

WzPackage::~WzPackage()
{
}

WzNameSpace* WzPackage::GetItem(const std::string& sName)
{
	auto pItem = WzNameSpace::GetItem(sName);
	if (pItem)
		pItem->OnGetItem();

	return pItem;
}

void WzPackage::OnGetItem()
{
	if (!m_bParsed) 
		LoadDirectory();
}

void WzPackage::LoadSubItem()
{
	for (auto& prChild : m_mChild)
		if (prChild.second->GetNameSpaceType() == WzNameSpaceType::Type_Property)
			prChild.second->OnGetItem();
}

bool WzPackage::LoadDirectory(bool bTestKey)
{
	WzStreamType wzStream(*m_pArchive->GetStream());
	wzStream.SetPosition(m_uBeginPos);

	int nCount = wzStream.ReadFilter<int>(), nType = 0, nOffset = 0;
	std::string sName;

	if (nCount < 0 || nCount > 65536)
		return false;
	for (int i = 0; i < nCount; ++i)
	{
		nType = 0;
		wzStream.Read((char*)&nType, 1);

		//pArchive in DecodeString <PCOM.dll>
		if (nType <= 2)
		{
			wzStream.Read((char*)&nOffset, 4); //ReadFully
			auto uCurrentPos = wzStream.GetPosition();
			wzStream.SetPosition(m_pArchive->GetBeginPos() + nOffset);
			wzStream.Read((char*)&nType, 1);
			sName = m_pArchive->DecodeString(&wzStream);
			wzStream.SetPosition(uCurrentPos);

			nType = WzNameSpaceType::Type_Property;
		}
		else
			sName = m_pArchive->DecodeString(&wzStream);

		wzStream.ReadFilter<int>();
		wzStream.ReadFilter<int>();
		auto uPos = LoadPos(&wzStream, m_pArchive->GetBeginPos(), m_pArchive->GetArchiveKey());

		if (bTestKey && uPos > m_pArchive->GetLength())
			return false;
		else if(bTestKey)
			return true;

		if (!bTestKey)
		{
			if (nType == WzNameSpaceType::Type_Directory)
				m_mChild.insert({ sName, new (WzPackage)(m_pArchive, (WzNameSpaceType)nType, sName, uPos) });
			else if (nType == WzNameSpaceType::Type_Property) 
			{
				//TRIMMED OUT .IMG
				if (sName.find(".img") != sName.npos)
					sName = sName.substr(0, sName.length() - 4);
				m_mChild.insert({ sName, new (WzNameSpaceProperty)(m_pArchive, (WzNameSpaceType)nType, sName, uPos) });
			}
		}
	}
	m_bParsed = true;
	return true;
}

unsigned int WzPackage::LoadPos(void *pStream, unsigned int uBegin, unsigned int uKey)
{
	unsigned p = ~(((WzStreamType*)pStream)->GetPosition() - uBegin);
	p *= uKey;
	p -= 0x581C3F6D;
	p = (p << (p & 0x1F)) | (p >> (32 - p & 0x1F));

	((WzStreamType*)pStream)->Read((char*)&uKey, 4);
	p ^= uKey;
	p += uBegin * 2;
	return p;
}
