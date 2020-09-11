#include "Rand32.h"
#include <random>
#include <ctime>

Rand32::Rand32()
	: m_Rand(rd), m_u(0, 2006674111), m_u2(0, 1721)
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
	std::lock_guard<std::mutex> lock(m_mtxLock);
	return (unsigned long long int)(m_u(m_Rand) ^ m_u(m_Rand) ^ m_u(m_Rand)) *(m_u2(m_Rand));
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

SynchronizedRand32::SynchronizedRand32()
{
	srand((unsigned int)time(nullptr));

	unsigned long long int v2 = rand();
	//double v2 = ((double)rand() / (double)RAND_MAX);
	Seed((unsigned int)(1170746341 * v2 - 755606699),
		(unsigned int)(1170746341 * v2 - 755606699),
		(unsigned int)(1170746341 * v2 - 755606699));
}

void SynchronizedRand32::Seed(unsigned int s1, unsigned int s2, unsigned int s3)
{
	std::lock_guard<std::mutex> lock(m_mtxLock);
	m_s1 = s1 | 0x100000;
	m_past_s1 = s1 | 0x100000;
	m_s2 = s2 | 0x1000;
	m_past_s2 = s2 | 0x1000;
	m_s3 = s3 | 0x10;
	m_past_s3 = s3 | 0x10;
}

SynchronizedRand32 * SynchronizedRand32::GetInstance()
{
	static SynchronizedRand32* pInstance = new SynchronizedRand32();
	return pInstance;
}

unsigned int SynchronizedRand32::Random()
{
	std::lock_guard<std::mutex> lock(m_mtxLock);
	m_past_s1 = m_s1;
	m_past_s2 = m_s2;
	m_past_s3 = m_s3;
	m_s1 = ((m_s1 & 0xFFFFFFFE) << 12) ^ ((m_s1 & 0x7FFC0 ^ (m_s1 >> 13)) >> 6);
	m_s2 = 16 * (m_s2 & 0xFFFFFFF8) ^ (((m_s2 >> 2) ^ m_s2 & 0x3F800000) >> 23);
	m_s3 = ((m_s3 & 0xFFFFFFF0) << 17) ^ (((m_s3 >> 3) ^ m_s3 & 0x1FFFFF00) >> 8);
	return  m_s1 ^ m_s2 ^ m_s3;
}

unsigned int SynchronizedRand32::Random(unsigned int nMin, unsigned int nMax)
{
	unsigned int nRndRange = (nMax - nMin);
	if (nRndRange == 0)
		return 0;
	return nMin + Random() % (nMax - nMin);
}
