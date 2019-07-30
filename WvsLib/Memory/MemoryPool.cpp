#include "MemoryPool.h"

/*MemoryPool<char[32]>  WvsArrayAllocator::__gMemPool32{ 32 * 2048 };
MemoryPool<char[64]>  WvsArrayAllocator::__gMemPool64{ 64 * 1024 };
MemoryPool<char[128]> WvsArrayAllocator::__gMemPool128{ 128 * 512 };
MemoryPool<char[256]> WvsArrayAllocator::__gMemPool256{ 256 * 512 };
MemoryPool<char[512]> WvsArrayAllocator::__gMemPool512{ 512 * 512 };*/

//For object allocations.
MemoryPool<char[32]> WVS_ALLOCATOR::__gsg0{ 32 * 2048 };
MemoryPool<char[64]> WVS_ALLOCATOR::__gsg1{ 64 * 1024 };
MemoryPool<char[128]> WVS_ALLOCATOR::__gsg2{ 128 * 512 };
MemoryPool<char[256]> WVS_ALLOCATOR::__gsg3{ 256 * 512 };
MemoryPool<char[512]> WVS_ALLOCATOR::__gsg4{ 512 * 512 };
MemoryPool<char[1024]> WVS_ALLOCATOR::__gsg5{ 1024 * 512 };
MemoryPool<char[2048]> WVS_ALLOCATOR::__gsg6{ 2048 * 512 };
MemoryPool<char[4096]> WVS_ALLOCATOR::__gsg7{ 4096 * 512 };
MemoryPool<char[8192]> WVS_ALLOCATOR::__gsg8{ 8192 * 512 };
MemoryPool<char[8192 * 2]> WVS_ALLOCATOR::__gsg9{ 8192 * 2 * 512 };
MemoryPool<char[8192 * 4]> WVS_ALLOCATOR::__gsg10{ 8192 * 4 * 512 };
MemoryPool<char[8192 * 8]> WVS_ALLOCATOR::__gsg11{ 8192 * 8 * 512 };

//For array allocations.
MemoryPool<char[32]> WVS_ALLOCATOR::__gMemPool32{ 32 * 2048 };
MemoryPool<char[64]> WVS_ALLOCATOR::__gMemPool64{ 64 * 1024 };
MemoryPool<char[128]> WVS_ALLOCATOR::__gMemPool128{ 128 * 512 };
MemoryPool<char[256]> WVS_ALLOCATOR::__gMemPool256{ 256 * 512 };
MemoryPool<char[512]> WVS_ALLOCATOR::__gMemPool512{ 512 * 512 };
MemoryPool<char[1024]> WVS_ALLOCATOR::__gMemPool1024{ 1024 * 512 };
MemoryPool<char[2048]> WVS_ALLOCATOR::__gMemPool2048{ 2048 * 512 };

void* WVS_ALLOCATOR::apAllocator[12] =
{
	&WVS_ALLOCATOR::__gsg0, &WVS_ALLOCATOR::__gsg1, &WVS_ALLOCATOR::__gsg2, &WVS_ALLOCATOR::__gsg3,
	&WVS_ALLOCATOR::__gsg4, &WVS_ALLOCATOR::__gsg5, &WVS_ALLOCATOR::__gsg6, &WVS_ALLOCATOR::__gsg7,
	&WVS_ALLOCATOR::__gsg8, &WVS_ALLOCATOR::__gsg9, &WVS_ALLOCATOR::__gsg10, &WVS_ALLOCATOR::__gsg11
};
