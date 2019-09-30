#include "WzProperty.h"
#include "WzArchive.h"
#include "..\Memory\MemoryPoolMan.hpp"
#include <iostream>

WzProperty::WzProperty(const std::string& sPropName)
	: WzNameSpace(nullptr, WzNameSpaceType::Type_Property, sPropName, 0)
{
	m_sName = sPropName;
	m_pArchive = nullptr;
	m_uBeginPos = 0;
}

WzProperty::WzProperty(WzArchive* pArchive, const std::string& sPropName, unsigned int uBeginPos, unsigned int uRootPropPos)
	: WzNameSpace(pArchive, WzNameSpaceType::Type_Property, sPropName, uBeginPos)
{
	m_pArchive = pArchive;
	m_uBeginPos = uBeginPos;
	WzStreamType wzStream(*m_pArchive->GetStream());
	wzStream.SetPosition(uBeginPos);

	int nHeader = 0;
	std::string sType = m_pArchive->DecodePropString(&wzStream, uRootPropPos), sName;
	if (sType == "Property")
	{
		wzStream.Read((char*)&nHeader, 2);

		int nCount = wzStream.ReadFilter<int>(), nType = 0;
		unsigned uOffset = 0, uNextPos = 0;
		for (int i = 0; i < nCount; ++i)
		{
			m_wzVariant.uData.liData = 0;
			nType = 0;
			nHeader = 0;
			sName = m_pArchive->DecodePropString(&wzStream, uRootPropPos);
			wzStream.Read((char*)&nHeader, 1);
			m_wzVariant.fType = nHeader;

			WzProperty* pSubItem = nullptr;
			if (nHeader == 9) //perform resursive parsing.
			{
				wzStream.Read((char*)&uOffset, 4);
				uNextPos = wzStream.GetPosition() + uOffset;
				pSubItem = new (WzProperty)(m_pArchive, sName, wzStream.GetPosition(), uRootPropPos);
				wzStream.SetPosition(uNextPos);
			}
			else
			{
				pSubItem = new (WzProperty)(sName);
				switch (nHeader)
				{
					case 19:
					case 3:
						pSubItem->m_wzVariant.vType = WzDelayedVariant::VariantType::vt_Filtered_Integer;
						pSubItem->m_wzVariant.uData.liData = wzStream.ReadFilter<int>();
						break;
					case 20:
						pSubItem->m_wzVariant.vType = WzDelayedVariant::VariantType::vt_Filtered_Long;
						pSubItem->m_wzVariant.uData.liData = wzStream.ReadFilter<unsigned long long int>();
						break;
					case 2:
					case 11:
						pSubItem->m_wzVariant.vType = WzDelayedVariant::VariantType::vt_Int16;
						pSubItem->m_wzVariant.uData.liData = 0;
						wzStream.Read((char*)&pSubItem->m_wzVariant.uData.liData, 2);
						break;
					case 4:
						pSubItem->m_wzVariant.vType = WzDelayedVariant::VariantType::vt_Float32;
						wzStream.Read((char*)&pSubItem->m_wzVariant.uData.liData, 1);
						if (pSubItem->m_wzVariant.uData.liData == 0x80)
							wzStream.Read((char*)&pSubItem->m_wzVariant.uData.fData, 4);
						else
							pSubItem->m_wzVariant.uData.fData = 0;
						break;
					case 5:
						pSubItem->m_wzVariant.vType = WzDelayedVariant::VariantType::vt_Double64;
						wzStream.Read((char*)&pSubItem->m_wzVariant.uData.dData, 8);
						break;
					case 8:
						pSubItem->m_wzVariant.vType = WzDelayedVariant::VariantType::vt_String;
						pSubItem->m_wzVariant.sData = m_pArchive->DecodePropString(&wzStream, uRootPropPos);
						break;
				}
			}
			m_mChild.insert({ sName, pSubItem });
		}
	}
	if (sType == "Canvas") {}
	else if (sType == "Shape2D#Vector2D") 
	{
		auto pX = new (WzProperty)("x");
		auto pY = new (WzProperty)("y");
		pX->m_wzVariant.vType = pY->m_wzVariant.vType = WzDelayedVariant::VariantType::vt_Filtered_Integer;
		pX->m_wzVariant.uData.liData = wzStream.ReadFilter<int>();
		pY->m_wzVariant.uData.liData = wzStream.ReadFilter<int>();

		m_mChild.insert({ "x", pX });
		m_mChild.insert({ "y", pY });
	}
	else if (sType == "Shape2D#Convex2D") {}
	else if (sType == "Sound_DX8") {}
	else if (sType == "UOL") {}

}

WzProperty::~WzProperty()
{
}

void WzProperty::OnGetItem()
{
}

WzDelayedVariant& WzProperty::GetVariant()
{
	return m_wzVariant;
}

WzProperty* WzProperty::GetProperty()
{
	return this;
}
