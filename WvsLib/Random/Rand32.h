#pragma once
#include <mutex>
#include <vector>
#include <random>

class PCGImpl
{
public:
	using result_type = uint32_t;
	static constexpr result_type(min)() { return 0; }
	static constexpr result_type(max)() { return UINT32_MAX; }

	PCGImpl()
		: m_uliState(0x853c49e6748fea9bULL)
		, m_uliInc(0xda3e39cb94b95bdbULL)
	{}

	explicit PCGImpl(std::random_device &rd)
	{
		Seed(rd);
	}

	void Seed(std::random_device &rd)
	{
		uint64_t s0 = uint64_t(rd()) << 31 | uint64_t(rd());
		uint64_t s1 = uint64_t(rd()) << 31 | uint64_t(rd());

		m_uliState = 0;
		m_uliInc = (s1 << 1) | 1;
		(void)operator()();
		m_uliState += s0;
		(void)operator()();
	}

	result_type operator()()
	{
		uint64_t uliOldstate = m_uliState;
		m_uliState = uliOldstate * 6364136223846793005ULL + m_uliInc;
		uint32_t uXorshifted = uint32_t(((uliOldstate >> 18u) ^ uliOldstate) >> 27u);
		int rot = uliOldstate >> 59u;
		return (uXorshifted >> rot) | (uXorshifted << ((-rot) & 31));
	}

	void Discard(unsigned long long n)
	{
		for (unsigned long long i = 0; i < n; ++i)
			operator()();
	}

private:
	uint64_t m_uliState;
	uint64_t m_uliInc;
};

/*This Rand32 is more uniformly distributed but does not synchronize with client.*/
/*Use this for reward probability calculation not for damage calculation which requires WVS version PRNG.*/
class Rand32
{
	std::mutex m_mtxLock;
	std::random_device rd;
	PCGImpl m_Rand;
	std::uniform_int_distribution<> m_u, m_u2;

public:
	Rand32();
	static Rand32* GetInstance();
	unsigned long long int Random();
	unsigned long long int Random(unsigned int nMin, unsigned int nMax);
	std::vector<int> GetRandomUniqueArray(int nStart, int nRange, int nCount);
};

class SynchronizedRand32
{
	std::mutex m_mtxLock;
	unsigned int m_s1, m_s2, m_s3, m_past_s1, m_past_s2, m_past_s3;

public:
	SynchronizedRand32();
	void Seed(unsigned int s1, unsigned int s2, unsigned int s3);
	static SynchronizedRand32* GetInstance();
	unsigned int Random();
	unsigned int Random(unsigned int nMin, unsigned int nMax);
};