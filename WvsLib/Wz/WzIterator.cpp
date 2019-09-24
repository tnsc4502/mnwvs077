#include "WzIterator.h"

WzIterator::WzIterator()
{
	m_pIterProp = nullptr;
	m_pIterNS = nullptr;
}

WzIterator::WzIterator(WzProperty* pNextProp, WzProperty::PropContainerType::iterator& it, WzProperty::PropContainerType::iterator& itEnd)
{
	m_pIterProp = pNextProp;
	m_pIterNS = nullptr;
	m_itProp = it;
	++m_itProp;
	m_itEndProp = itEnd;
}

WzIterator::WzIterator(WzNameSpace* pNextNS, WzNameSpace::NameSpaceContainerType::iterator& it, WzNameSpace::NameSpaceContainerType::iterator& itEnd)
{
	m_pIterProp = nullptr;
	m_pIterNS = pNextNS;
	m_itNS = it;
	++m_itNS;
	m_itEndNS = itEnd;
}

WzIterator::WzIterator(WzNameSpace *pTopProperty)
{
	if (pTopProperty->GetProperty())
	{
		m_pIterNS = nullptr;
		m_pIterProp = pTopProperty->GetProperty();
		m_itProp = m_pIterProp->m_mChild.begin();
	}
	else
	{
		m_pIterProp = nullptr;
		m_pIterNS = pTopProperty;
		m_itNS = m_pIterNS->m_mChild.begin();
	}
}

WzIterator::~WzIterator()
{
}

WzIterator WzIterator::operator[](const std::string& sName)
{
	if (!m_pIterProp && !m_pIterNS)
		return end();

	auto pProp = m_pIterProp;
	if (pProp || (pProp = m_pIterNS->GetProperty()))
	{
		auto findIter = pProp->m_mChild.find(sName);
		if (findIter == pProp->m_mChild.end())
			return end();

		return WzIterator(findIter->second, findIter, pProp->m_mChild.end());
	}
	else
	{
		auto findIter = m_pIterNS->m_mChild.find(sName);
		if (findIter == m_pIterNS->m_mChild.end())
			return end();

		findIter->second->OnGetItem();
		return WzIterator(findIter->second, findIter, m_pIterNS->m_mChild.end());
	}
}

WzIterator WzIterator::operator[](const char* sName)
{
	return operator[](std::string(sName));
}

WzIterator WzIterator::begin()
{
	auto pProp = m_pIterProp;
	if (pProp || (m_pIterNS && (pProp = m_pIterNS->GetProperty())))
	{
		if (pProp->m_mChild.size() == 0)
			return end();

		auto pChild = pProp->m_mChild.begin()->second;
		return WzIterator(pChild, pProp->m_mChild.begin(), pProp->m_mChild.end());
	}
	else if(m_pIterNS)
	{
		/*if (m_pIterNS->GetProperty())
			return WzIterator(m_pIterNS);*/

		if (m_pIterNS->m_mChild.size() == 0)
			return end();

		auto pChild = m_pIterNS->m_mChild.begin()->second;
		pChild->OnGetItem();
		return WzIterator(pChild, m_pIterNS->m_mChild.begin(), m_pIterNS->m_mChild.end());
	}
	return end();
}

WzIterator WzIterator::end()
{
	return WzIterator();
}

WzIterator& WzIterator::operator++()
{
	if (m_pIterProp)
	{
		if (m_itProp == m_itEndProp)
			*this = end();
		else
		{
			m_pIterProp = m_itProp->second;
			++m_itProp;
		}
	}
	else if (m_pIterNS)
	{
		if (m_itNS == m_itEndNS)
			*this = end();
		else
		{
			m_pIterNS = m_itNS->second;
			m_pIterNS->OnGetItem();
			++m_itNS;
		}
	}
	return *this;
}

WzIterator& WzIterator::operator*()
{
	return *this;
}

const std::string & WzIterator::GetName()
{
	return m_pIterProp ? m_pIterProp->GetName() : m_pIterNS->GetName();
}

bool WzIterator::operator!=(const WzIterator & rhs)
{
	return (m_pIterProp != rhs.m_pIterProp || m_pIterNS != rhs.m_pIterNS);
}

bool WzIterator::operator==(const WzIterator& rhs)
{
	return !operator!=(rhs);
}

WzIterator::operator int()
{
	if (!m_pIterProp)
		return 0;
	auto& m_wzVariant = m_pIterProp->GetVariant();
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
	if (!m_pIterProp)
		return 0;
	auto& m_wzVariant = m_pIterProp->GetVariant();
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
	if (!m_pIterProp)
		return 0;
	auto& m_wzVariant = m_pIterProp->GetVariant();
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
	if (!m_pIterProp)
		return sEmpty;
	auto& m_wzVariant = m_pIterProp->GetVariant();
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
