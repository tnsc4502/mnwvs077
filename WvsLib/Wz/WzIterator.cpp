#include "WzIterator.h"

WzIterator::WzIterator()
{
	m_pIterNS = nullptr;
}

WzIterator::WzIterator(WzNameSpace* pNextNS, WzNameSpace::NameSpaceContainerType::iterator& it, WzNameSpace::NameSpaceContainerType::iterator& itEnd)
{
	m_pIterNS = pNextNS;
	m_itNS = it;
	++m_itNS;
	m_itEndNS = itEnd;
}

WzIterator::WzIterator(WzNameSpace *pTopProperty)
{
	m_pIterNS = pTopProperty;
	m_itNS = m_pIterNS->m_mChild.begin();
	m_itEndNS = m_pIterNS->m_mChild.end();
}

WzIterator::~WzIterator()
{
}

WzIterator WzIterator::operator[](const std::string& sName)
{
	if (!m_pIterNS)
		return end();

	auto pNameSpace = (m_pIterNS->GetProperty() ? m_pIterNS->GetProperty() : m_pIterNS);

	auto findIter = pNameSpace->m_mChild.find(sName);
	if (findIter == pNameSpace->m_mChild.end())
		return end();

	findIter->second->OnGetItem();
	return WzIterator(findIter->second, findIter, pNameSpace->m_mChild.end());
}

WzIterator WzIterator::operator[](const char* sName)
{
	return operator[](std::string(sName));
}

WzIterator WzIterator::begin()
{
	auto pProp = (m_pIterNS && m_pIterNS->GetProperty() ? m_pIterNS->GetProperty() : m_pIterNS);

	if (!pProp || pProp->m_mChild.size() == 0)
		return end();

	auto pChild = pProp->m_mChild.begin()->second;
	pChild->OnGetItem();
	return WzIterator(pChild, pProp->m_mChild.begin(), pProp->m_mChild.end());
}

const WzIterator& WzIterator::end()
{
	static WzIterator empty;
	return empty;
}

WzIterator& WzIterator::operator++()
{
	if (m_itNS == m_itEndNS)
		*this = end();
	else
	{
		m_pIterNS = m_itNS->second;
		m_pIterNS->OnGetItem();
		++m_itNS;
	}

	return *this;
}

WzIterator& WzIterator::operator*()
{
	return *this;
}

const std::string & WzIterator::GetName()
{
	return m_pIterNS->GetName();
}

bool WzIterator::operator!=(const WzIterator & rhs)
{
	return !operator==(rhs);
}

bool WzIterator::operator==(const WzIterator & rhs)
{
	return (m_pIterNS == rhs.m_pIterNS);
}

WzIterator::operator int()
{
	WzProperty* pProp = (m_pIterNS ? m_pIterNS->GetProperty() : nullptr);
	if (!pProp)
		return 0;

	auto& m_wzVariant = pProp->GetVariant();
	if (m_wzVariant.vType == WzDelayedVariant::vt_Filtered_Long ||
		m_wzVariant.vType == WzDelayedVariant::vt_Filtered_Integer ||
		m_wzVariant.vType == WzDelayedVariant::vt_Int16)
		return (int)m_wzVariant.uData.liData;

	if (m_wzVariant.vType == WzDelayedVariant::vt_String)
		return atoi(m_wzVariant.sData.c_str());
	else if (m_wzVariant.vType == WzDelayedVariant::vt_Double64)
		return (int)m_wzVariant.uData.dData;
	else if (m_wzVariant.vType == WzDelayedVariant::vt_Float32)
		return (int)m_wzVariant.uData.fData;

	return (int)m_wzVariant.uData.liData;
}

WzIterator::operator unsigned long long()
{
	WzProperty* pProp = (m_pIterNS ? m_pIterNS->GetProperty() : nullptr);
	if (!pProp)
		return 0;

	auto& m_wzVariant = pProp->GetVariant();
	if (m_wzVariant.vType == WzDelayedVariant::vt_Filtered_Long ||
		m_wzVariant.vType == WzDelayedVariant::vt_Filtered_Integer ||
		m_wzVariant.vType == WzDelayedVariant::vt_Int16)
		return m_wzVariant.uData.liData;

	if (m_wzVariant.vType == WzDelayedVariant::vt_String)
		return atoll(m_wzVariant.sData.c_str());
	else if (m_wzVariant.vType == WzDelayedVariant::vt_Double64)
		return (unsigned long long)m_wzVariant.uData.dData;
	else if (m_wzVariant.vType == WzDelayedVariant::vt_Float32)
		return (unsigned long long)m_wzVariant.uData.fData;

	return m_wzVariant.uData.liData;
}

WzIterator::operator double()
{
	WzProperty* pProp = (m_pIterNS ? m_pIterNS->GetProperty() : nullptr);
	if (!pProp)
		return 0;

	auto& m_wzVariant = pProp->GetVariant();
	if (m_wzVariant.vType == WzDelayedVariant::vt_Double64)
		return m_wzVariant.uData.dData;

	if (m_wzVariant.vType == WzDelayedVariant::vt_String)
		return atof(m_wzVariant.sData.c_str());
	else if (m_wzVariant.vType == WzDelayedVariant::vt_Float32)
		return (float)m_wzVariant.uData.fData;
	else if (m_wzVariant.vType != WzDelayedVariant::vt_Float32)
		return (float)m_wzVariant.uData.liData;

	return 0;
}

WzIterator::operator float()
{
	return (float)operator double();
}

WzIterator::operator const std::string&()
{
	static std::string sEmpty;
	WzProperty* pProp = (m_pIterNS ? m_pIterNS->GetProperty() : nullptr);
	if (!pProp)
		return sEmpty;

	auto& m_wzVariant = pProp->GetVariant();
	if (m_wzVariant.vType == WzDelayedVariant::vt_String)
		return m_wzVariant.sData;

	if (m_wzVariant.sData == "")
	{
		if (m_wzVariant.vType == WzDelayedVariant::vt_Filtered_Long ||
			m_wzVariant.vType == WzDelayedVariant::vt_Filtered_Integer ||
			m_wzVariant.vType == WzDelayedVariant::vt_Int16)
			m_wzVariant.sData = std::to_string(m_wzVariant.uData.liData);
		else if (m_wzVariant.vType == WzDelayedVariant::vt_Float32)
			m_wzVariant.sData = std::to_string(m_wzVariant.uData.fData);
		else
			m_wzVariant.sData = std::to_string(m_wzVariant.uData.dData);
	}
	return m_wzVariant.sData;
}

std::vector<std::string> WzIterator::EnumerateChildName()
{
	std::vector<std::string> aRet;
	auto pNameSpace = (m_pIterNS->GetProperty() ? m_pIterNS->GetProperty() : m_pIterNS);

	if (pNameSpace)
		for (auto& prChild : pNameSpace->m_mChild)
			aRet.push_back(prChild.first);
	return aRet;
}
