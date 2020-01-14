#include "WzProperty.h"
#include "WzArchive.h"
#include "..\Memory\MemoryPoolMan.hpp"
#include <list>

WzProperty::WzProperty(const std::string& sPropName)
	: WzNameSpace(nullptr, WzNameSpaceType::Type_Property, sPropName, 0)
{
	m_sName = sPropName;
	m_pArchive = nullptr;
	m_uBeginPos = 0;
}

void WzProperty::DelayParse()
{
	WzStreamType wzStream(*m_pArchive->GetStream());
	wzStream.SetPosition(m_uBeginPos);

	int nHeader = 0, nCount = 0, nType = 0;
	unsigned uOffset = 0;
	WzProperty *pSubItem = nullptr;
	std::string sType = m_pArchive->DecodePropString(&wzStream, m_uRootPropPos), sName;
	if (sType[0] == 'P')
	{
		wzStream.Read((char*)&nHeader, 2);

		nCount = wzStream.ReadFilter<int>();
		uOffset = 0;
		for (int i = 0; i < nCount; ++i)
		{
			m_wzVariant.uData.liData = 0;
			nType = 0;
			nHeader = 0;
			sName = m_pArchive->DecodePropString(&wzStream, m_uRootPropPos);
			wzStream.Read((char*)&nHeader, 1);
			m_wzVariant.fType = nHeader;

			if (nHeader == 9)
			{
				wzStream.Read((char*)&uOffset, 4);
				pSubItem = AllocObjCtor (WzProperty)(m_pArchive, sName, wzStream.GetPosition(), m_uRootPropPos);
				wzStream.SetPosition(wzStream.GetPosition() + uOffset);
			}
			else //Delayed value props.
			{
				pSubItem = AllocObjCtor(WzProperty)(sName);
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
						pSubItem->m_wzVariant.sData = m_pArchive->DecodePropString(&wzStream, m_uRootPropPos);
						break;
				}
			}

			m_mChild.insert({ sName, pSubItem });
		}
	}
	//else if (sType == "Canvas") {}
	else if (sType == "Shape2D#Vector2D")
	{
		auto pX = AllocObjCtor(WzProperty)("x");
		auto pY = AllocObjCtor(WzProperty)("y");
		pX->m_wzVariant.vType = pY->m_wzVariant.vType = WzDelayedVariant::VariantType::vt_Filtered_Integer;
		pX->m_wzVariant.uData.liData = wzStream.ReadFilter<int>();
		pY->m_wzVariant.uData.liData = wzStream.ReadFilter<int>();

		m_mChild.insert({ "x", pX });
		m_mChild.insert({ "y", pY });
	}
	//mDeepProp.erase(iter);

	m_bParsed = true;
}

WzProperty::WzProperty(WzArchive* pArchive, const std::string& sPropName, unsigned int uBeginPos, unsigned int uRootPropPos)
	: WzNameSpace(pArchive, WzNameSpaceType::Type_Property, sPropName, uBeginPos)
{
	m_uRootPropPos = uRootPropPos;


	/*WzStreamType wzStream(*m_pArchive->GetStream());
	//wzStream.SetPosition(uBeginPos);

	int nHeader = 0, nCount = 0, nType = 0;
	unsigned uOffset = 0;
	WzProperty *pProp, *pSubItem = nullptr;
	std::list<std::pair<WzProperty*, unsigned int>> aProp;
	aProp.push_back({ this, m_uBeginPos });
	while (!aProp.empty())
	{
		auto uPos = aProp.front().second;
		pProp = aProp.front().first;
		wzStream.SetPosition(uPos);

		std::string sType = pArchive->DecodePropString(&wzStream, uRootPropPos), sName;
		if (sType[0] == 'P')
		{
			wzStream.Read((char*)&nHeader, 2);

			nCount = wzStream.ReadFilter<int>();
			uOffset = 0;
			for (int i = 0; i < nCount; ++i)
			{
				m_wzVariant.uData.liData = 0;
				nType = 0;
				nHeader = 0;
				sName = pArchive->DecodePropString(&wzStream, uRootPropPos);
				wzStream.Read((char*)&nHeader, 1);
				m_wzVariant.fType = nHeader;

				pSubItem = new (WzProperty)(sName);
				switch (nHeader)
				{
				case 9:
					wzStream.Read((char*)&uOffset, 4);
					aProp.push_back({ pSubItem, wzStream.GetPosition() });
					wzStream.SetPosition(wzStream.GetPosition() + uOffset);
					break;
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
					pSubItem->m_wzVariant.sData = pArchive->DecodePropString(&wzStream, uRootPropPos);
					break;
				}
				pProp->m_mChild.insert({ sName, pSubItem });
			}
		}
		//else if (sType == "Canvas") {}
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
		aProp.pop_front();
	}*/
}

WzProperty::~WzProperty()
{
	for (auto& prChild : m_mChild)
		WvsSingleObjectAllocator<WzProperty>::GetInstance()->Free(prChild.second);

	m_mChild.clear(); //Prevent releasing again from WzNameSpace
}

void WzProperty::OnGetItem()
{
	if (!m_bParsed && m_wzVariant.vType == WzDelayedVariant::vt_None)
		DelayParse();
}

WzDelayedVariant& WzProperty::GetVariant()
{
	return m_wzVariant;
}

WzProperty* WzProperty::GetProperty()
{
	return this;
}
