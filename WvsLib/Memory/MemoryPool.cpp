#include "MemoryPool.h"

MemoryPoolMan::MemoryPoolMan()
{
	//Initialize single object pools.
	MemoryPool<char> *pSmallObjPool = nullptr;
	for (int i = 0; i < MAX_POOL_NUM; ++i)
	{
		if (i == 0)
		{
			m_apPool[i] = new MemoryPool<char>(32, 32 * INITIAL_BLOCK_NUM);
			pSmallObjPool = m_apPool[i];
		}
		else if (i <= 5)
			m_apPool[i] = pSmallObjPool;
		else
			m_apPool[i] = new MemoryPool<char>(
			(int)std::pow(2, i), 
			(int)std::pow(2, i) * INITIAL_BLOCK_NUM
			);
	}

	//Initialize array pools
	MemoryPool<char> *pSmallArrayPool = nullptr;
	for (int i = 0; i < MAX_POOL_NUM; ++i)
	{
		if (i == 0)
		{
			m_apArrayPool[i] = new MemoryPool<char>(32, 32 * INITIAL_BLOCK_NUM);
			pSmallArrayPool = m_apArrayPool[i];
		}
		else if (i <= 5)
			m_apArrayPool[i] = pSmallArrayPool;
		else
			m_apArrayPool[i] = new MemoryPool<char>(
			(int)std::pow(2, i),
			(int)std::pow(2, i) * INITIAL_BLOCK_NUM);

		if (i == 0 || i > 5)
		{
			//for (int j = 0; j < INITIAL_BLOCK_NUM; ++j)
			//	m_apArrayPool[i]->allocateBlock();
		}
	}
}

MemoryPool<char>* MemoryPoolMan::GetPool(size_t nPos)
{
	return m_apPool[nPos];
}

MemoryPool<char>* MemoryPoolMan::GetArrayPool(size_t nPos)
{
	return m_apPool[nPos];
}
