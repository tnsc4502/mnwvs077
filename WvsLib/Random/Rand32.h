#pragma once
#include <mutex>
#include <vector>
#include <random>

class WvsPCG
{
public:
	using result_type = uint32_t;
	static constexpr result_type(min)() { return 0; }
	static constexpr result_type(max)() { return UINT32_MAX; }

	WvsPCG()
		: m_state(0x853c49e6748fea9bULL)
		, m_inc(0xda3e39cb94b95bdbULL)
	{}

	explicit WvsPCG(std::random_device &rd)
	{
		seed(rd);
	}

	void seed(std::random_device &rd)
	{
		uint64_t s0 = uint64_t(rd()) << 31 | uint64_t(rd());
		uint64_t s1 = uint64_t(rd()) << 31 | uint64_t(rd());

		m_state = 0;
		m_inc = (s1 << 1) | 1;
		(void)operator()();
		m_state += s0;
		(void)operator()();
	}

	result_type operator()()
	{
		uint64_t oldstate = m_state;
		m_state = oldstate * 6364136223846793005ULL + m_inc;
		uint32_t xorshifted = uint32_t(((oldstate >> 18u) ^ oldstate) >> 27u);
		int rot = oldstate >> 59u;
		return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
	}

	void discard(unsigned long long n)
	{
		for (unsigned long long i = 0; i < n; ++i)
			operator()();
	}

private:
	uint64_t m_state;
	uint64_t m_inc;
};

class Rand32
{
	std::mutex m_lock;
	std::random_device rd;
	WvsPCG rand;
	std::uniform_int_distribution<> u, u2;

public:
	Rand32();
	static Rand32* GetInstance();
	unsigned long long int Random();
	unsigned long long int Random(unsigned int nMin, unsigned int nMax);
	std::vector<int> GetRandomUniqueArray(int nStart, int nRange, int nCount);
};