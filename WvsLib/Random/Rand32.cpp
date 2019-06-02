#include "Rand32.h"
#include <random>
#include <ctime>

Rand32::Rand32()
	: rand(rd), u(0, 2006674111), u2(0, 1721)
{
	srand((unsigned int)time(nullptr));
}

/*void Rand32::Seed(unsigned int s1, unsigned int s2, unsigned int s3)
{
	std::lock_guard<std::mutex> lock(m_lock);
	m_s1 = s1 | 0x100000;
	m_past_s1 = s1 | 0x100000;
	m_s2 = s2 | 0x1000;
	m_past_s2 = s2 | 0x1000;
	m_s3 = s3 | 0x10;
	m_past_s3 = s3 | 0x10;
}*/

Rand32 * Rand32::GetInstance()
{
	static Rand32* pInstance = new Rand32();
	return pInstance;
}

unsigned long long int Rand32::Random()
{
	std::lock_guard<std::mutex> lock(m_lock);
	return (unsigned long long int)(u(rand) ^ u(rand) ^ u(rand)) *(u2(rand));
}

unsigned long long int Rand32::Random(unsigned int nMin, unsigned int nMax)
{
	unsigned int nRndRange = (nMax - nMin);
	if (nRndRange == 0)
		return 0;
	return nMin + GetInstance()->Random() % (nMax - nMin);
}

std::vector<int> Rand32::GetRandomUniqueArray(int nStart, int nRange, int nCount)
{
	std::vector<int> aRet;
	aRet.resize(nCount);
	std::vector<int> aSample;
	for (int i = 0; i < nRange; ++i)
		aSample.push_back(nStart + i);
	int nRnd = 0;
	for (int i = 0; i < nCount; ++i)
	{
		nRnd = (int)(Random() % aSample.size());
		aRet[i] = aSample[nRnd];
		aSample.erase(aSample.begin() + nRnd);
	}
	return aRet;
}
