#include "WzProperty.h"
#include "WzArchive.h"
#include "..\Memory\MemoryPoolMan.hpp"
#include <iostream>

WzProperty::WzProperty(const std::string& sPropName)
{
	m_sName = sPropName;
	m_pArchive = nullptr;
	m_uBeginPos = 0;
}

WzProperty::WzProperty(WzArchive* pArchive, const std::string& sPropName, unsigned int uBeginPos, unsigned int uRootPropPos)
	: m_sName(sPropName)
{
	m_pArchive = pArchive;
	m_uBeginPos = uBeginPos;

	int nHeader = 0;
	std::string sType = m_pArchive->DecodePropString(uRootPropPos), sName;
	if (sType == "Property")
	{
		m_pArchive->Read((char*)&nHeader, 2);

		int nCount = m_pArchive->ReadFilter<int>(), nType = 0;
		unsigned uOffset = 0, uNextPos = 0;
		for (int i = 0; i < nCount; ++i)
		{
			m_wzVariant.uData.liData = 0;
			nType = 0;
			nHeader = 0;
			sName = m_pArchive->DecodePropString(uRootPropPos);
			m_pArchive->Read((char*)&nHeader, 1);
			m_wzVariant.fType = nHeader;

			WzProperty* pSubItem = nullptr;
			if (nHeader == 9) //perform resursive parsing.
			{
				m_pArchive->Read((char*)&uOffset, 4);
				uNextPos = m_pArchive->GetPosition() + uOffset;
				pSubItem = AllocObjCtor(WzProperty)(m_pArchive, sName, m_pArchive->GetPosition(), uRootPropPos);
				m_pArchive->SetPosition(uNextPos);
			}
			else
			{
				pSubItem = AllocObjCtor(WzProperty)(sName);
				switch (nHeader)
				{
					case 9:

						break;
					case 19:
					case 3:
						pSubItem->m_wzVariant.vType = WzDelayedVariant::VariantType::vt_Filtered_Integer;
						pSubItem->m_wzVariant.uData.liData = m_pArchive->ReadFilter<int>();
						break;
					case 20:
						pSubItem->m_wzVariant.vType = WzDelayedVariant::VariantType::vt_Filtered_Long;
						pSubItem->m_wzVariant.uData.liData = m_pArchive->ReadFilter<unsigned long long int>();
						break;
					case 2:
					case 11:
						pSubItem->m_wzVariant.vType = WzDelayedVariant::VariantType::vt_Int16;
						pSubItem->m_wzVariant.uData.liData = 0;
						m_pArchive->Read((char*)&pSubItem->m_wzVariant.uData.liData, 2);
						break;
					case 4:
						pSubItem->m_wzVariant.vType = WzDelayedVariant::VariantType::vt_Float32;
						m_pArchive->Read((char*)&pSubItem->m_wzVariant.uData.liData, 1);
						if (pSubItem->m_wzVariant.uData.liData == 0x80)
							m_pArchive->Read((char*)&pSubItem->m_wzVariant.uData.fData, 4);
						else
							pSubItem->m_wzVariant.uData.fData = 0;
						break;
					case 5:
						pSubItem->m_wzVariant.vType = WzDelayedVariant::VariantType::vt_Double64;
						m_pArchive->Read((char*)&pSubItem->m_wzVariant.uData.dData, 8);
						break;
					case 8:
						pSubItem->m_wzVariant.vType = WzDelayedVariant::VariantType::vt_String;
						pSubItem->m_wzVariant.sData = m_pArchive->DecodePropString(uRootPropPos);
						break;
				}
			}
			m_mChild.insert({ sName, pSubItem });
		}
	}
	if (sType == "Canvas") {}
	else if (sType == "Shape2D#Vector2D") {}
	else if (sType == "Shape2D#Convex2D") {}
	else if (sType == "Sound_DX8") {}
	else if (sType == "UOL") {}

}

WzProperty::~WzProperty()
{
	for (auto& prChild : m_mChild)
		FreeObj(prChild.second);
}

const std::string & WzProperty::GetName()
{
	return m_sName;
}

WzProperty* WzProperty::GetItem(const std::string & sName)
{
	auto findIter = m_mChild.find(sName);
	return findIter == m_mChild.end() ? nullptr : findIter->second;
}

WzDelayedVariant& WzProperty::GetVariant()
{
	return m_wzVariant;
}