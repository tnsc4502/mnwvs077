#include "TemporaryStat.h"
#include "..\WvsLib\Net\OutPacket.h"

TemporaryStat::TS_Flag::TS_Flag()
{
	memset(
		m_aData, 0, sizeof(m_aData)
	);
	m_nFlagPos = 0;
	bEmpty = true;
	m_nTSValue = 0;
}

TemporaryStat::TS_Flag::TS_Flag(int dwFlagValue)
{
	bEmpty = false;
	m_nFlagPos = FLAG_COUNT - 1 - (dwFlagValue / 32);
	/*if (m_nFlagPos % 2)
		--m_nFlagPos;
	else
		++m_nFlagPos;
	m_nFlagPos = FLAG_COUNT - m_nFlagPos - 1;*/

	int nValue = (1 << ((dwFlagValue % 32)));
	for (int i = 0; i < FLAG_COUNT; ++i)
		m_aData[i] = 0;
	m_aData[m_nFlagPos] |= nValue;
	m_nTSValue = dwFlagValue;
}

void TemporaryStat::TS_Flag::Encode(OutPacket * oPacket)
{
	long long int *pFlag = (long long int *)m_aData;
	for (int i = 0; i < FLAG_COUNT; ++i) 
	{
		//printf("Encode TS Flag : [%d] = [%08X]\n", i, m_aData[i]);
		oPacket->Encode4(m_aData[i]);
	}
}

bool TemporaryStat::TS_Flag::IsIndieTS() const
{
	return false;
}

TemporaryStat::TS_Flag & TemporaryStat::TS_Flag::operator|=(const TS_Flag & rhs)
{
	if (bEmpty)
		*this = rhs;
	else
		m_aData[rhs.m_nFlagPos] |= rhs.m_aData[rhs.m_nFlagPos];
	bEmpty = false;
	return *this;
}

TemporaryStat::TS_Flag & TemporaryStat::TS_Flag::operator^=(const TS_Flag & rhs)
{
	if (bEmpty)
		*this = rhs;
	else
		m_aData[rhs.m_nFlagPos] &= ~(rhs.m_aData[rhs.m_nFlagPos]);
	bEmpty = false;
	return *this;
}

bool TemporaryStat::TS_Flag::operator&(const TS_Flag & rhs)
{
	return (m_aData[rhs.m_nFlagPos] & rhs.m_aData[rhs.m_nFlagPos]) != 0;
}

bool TemporaryStat::TS_Flag::IsEmpty() const
{
	return bEmpty;
}

TemporaryStat::TS_Flag TemporaryStat::TS_Flag::GetDefault()
{
	TS_Flag ret;
	return ret;
}
