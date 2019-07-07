/*-
* Copyright (c) 2013 Cosku Acay, http://www.coskuacay.com
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/

#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#pragma warning(disable:4624)  

#include <climits>
#include <cstddef>
#include <mutex>

template <typename T>
class MemoryPool
{
	std::mutex m_mtxLock;
public:
	/* Member types */
	typedef T               value_type;
	typedef T*              pointer;
	typedef T&              reference;
	typedef const T*        const_pointer;
	typedef const T&        const_reference;
	typedef size_t          size_type;
	typedef ptrdiff_t       difference_type;
	//   typedef std::false_type propagate_on_container_copy_assignment;
	//   typedef std::true_type  propagate_on_container_move_assignment;
	//   typedef std::true_type  propagate_on_container_swap;

	template <typename U> struct rebind {
		typedef MemoryPool<U> other;
	};

	size_t BlockSize;

	/* Member functions */
	MemoryPool(int nBlockSize) noexcept;
	MemoryPool(const MemoryPool& memoryPool) noexcept;
	MemoryPool(MemoryPool&& memoryPool) noexcept;
	template <class U> MemoryPool(const MemoryPool<U>& memoryPool) noexcept;

	~MemoryPool() noexcept;

	MemoryPool& operator=(const MemoryPool& memoryPool) = delete;
	MemoryPool& operator=(MemoryPool&& memoryPool) noexcept;

	pointer address(reference x) const noexcept;
	const_pointer address(const_reference x) const noexcept;

	// Can only allocate one object at a time. n and hint are ignored
	pointer allocate(size_type n = 1, const_pointer hint = 0);
	void deallocate(pointer p, size_type n = 1);

	size_type max_size() const noexcept;

	template <class U, class... Args> void construct(U* p, Args&&... args);
	template <class U> void destroy(U* p);

	template <class... Args> pointer newElement(Args&&... args);
	template <class U, class... Args> pointer newElementCtor(Args&&... args);
	void deleteElement(pointer p);

private:
	union Slot_ {
		value_type element;
		Slot_* next;
	};

	typedef char* data_pointer_;
	typedef Slot_ slot_type_;
	typedef Slot_* slot_pointer_;

	slot_pointer_ currentBlock_;
	slot_pointer_ currentSlot_;
	slot_pointer_ lastSlot_;
	slot_pointer_ freeSlots_;

	size_type padPointer(data_pointer_ p, size_type align) const noexcept;
	void allocateBlock();

	//static_assert(BlockSize >= 2 * sizeof(slot_type_), "BlockSize too small.");
};


#include "MemoryPool.tcc"
#include <type_traits>

#define BIND_ALLOCATOR(n, sz, n1) if(sz < n1) pRet = WVS_ALLOCATOR::__gsg##n.newElement(); else
#define BIND_ALLOCATOR_CTOR(n, sz, n1) if(sz < n1) pRet = WVS_ALLOCATOR::__gsg##n.newElementCtor<T>(std::forward<Args>(args)...); else
#define BIND_DEALLOCATOR(n, sz, n1)  if(sz < n1) WVS_ALLOCATOR::__gsg##n.deallocate((decltype(WVS_ALLOCATOR::__gsg##n)::pointer)pDel); else

namespace WVS_ALLOCATOR
{
	//For object allocations.
	static MemoryPool<char[32]>  __gsg0{ 32 * 2048 };
	static MemoryPool<char[64]>  __gsg1{ 64 * 1024 };
	static MemoryPool<char[128]> __gsg2{ 128 * 512 };
	static MemoryPool<char[256]> __gsg3{ 256 * 512 };
	static MemoryPool<char[512]> __gsg4{ 512 * 512 };
	static MemoryPool<char[1024]> __gsg5{ 1024 * 512 };
	static MemoryPool<char[2048]> __gsg6{ 2048 * 512 };
	static MemoryPool<char[4096]> __gsg7{ 4096 * 512 };
	static MemoryPool<char[8192]> __gsg8{ 8192 * 512 };
	static MemoryPool<char[8192 * 2]> __gsg9{ 8192 * 2 * 512 };
	static MemoryPool<char[8192 * 4]> __gsg10{ 8192 * 4 * 512 };
	static MemoryPool<char[8192 * 8]> __gsg11{ 8192 * 8 * 512 };

	//For array allocations.
	static MemoryPool<char[32]> __gMemPool32{ 32 * 2048 };
	static MemoryPool<char[64]> __gMemPool64{ 64 * 1024 };
	static MemoryPool<char[128]> __gMemPool128{ 128 * 512 };
	static MemoryPool<char[256]> __gMemPool256{ 256 * 512 };
	static MemoryPool<char[512]> __gMemPool512{ 512 * 512 };
	static MemoryPool<char[1024]> __gMemPool1024{ 1024 * 512 };
	static MemoryPool<char[2048]> __gMemPool2048{ 2048 * 512 };

	static void* apAllocator[12] =
	{
		&__gsg0, &__gsg1, &__gsg2, &__gsg3,
		&__gsg4, &__gsg5, &__gsg6, &__gsg7,
		&__gsg8, &__gsg9, &__gsg10, &__gsg11
	};
}

template<typename T>
class WvsSingleObjectAllocator
{
	std::mutex m_mtxLock;

public:
	inline void * ResourceMgr(bool allocate = true, void *pDel = nullptr)
	{
		std::lock_guard<std::mutex> lock__(m_mtxLock);
		const int SIZE = sizeof(T) + 4;
		void *pRet = nullptr;
		if (allocate)
		{
			if (SIZE > 8192 * 8)
				pRet = ::operator new(SIZE);
			else
			{
				BIND_ALLOCATOR(0, SIZE, 32)
				BIND_ALLOCATOR(1, SIZE, 64)
				BIND_ALLOCATOR(2, SIZE, 128)
				BIND_ALLOCATOR(3, SIZE, 256)
				BIND_ALLOCATOR(4, SIZE, 512)
				BIND_ALLOCATOR(5, SIZE, 1024)
				BIND_ALLOCATOR(6, SIZE, 2048)
				BIND_ALLOCATOR(7, SIZE, 4096)
				BIND_ALLOCATOR(8, SIZE, 8192)
				BIND_ALLOCATOR(9, SIZE, 8192 * 2)
				BIND_ALLOCATOR(10, SIZE, 8192 * 4)
				BIND_ALLOCATOR(11, SIZE, 8192 * 8);
			}
			if (pRet)
				*((int*)pRet) = SIZE;
		}
		else
		{
			int nRelease = *(int*)pDel;
			if (nRelease > 8192 * 8)
				::operator delete(pDel, nRelease);

			BIND_DEALLOCATOR(0, nRelease, 32)
			BIND_DEALLOCATOR(1, nRelease, 64)
			BIND_DEALLOCATOR(2, nRelease, 128)
			BIND_DEALLOCATOR(3, nRelease, 256)
			BIND_DEALLOCATOR(4, nRelease, 512)
			BIND_DEALLOCATOR(5, nRelease, 1024)
			BIND_DEALLOCATOR(6, nRelease, 2048)
			BIND_DEALLOCATOR(7, nRelease, 4096)
			BIND_DEALLOCATOR(8, nRelease, 8192)
			BIND_DEALLOCATOR(9, nRelease, 8192 * 2)
			BIND_DEALLOCATOR(10, nRelease, 8192 * 4)
			BIND_DEALLOCATOR(11, nRelease, 8192 * 8);
		}
		return pRet;
	}

public:
	template<bool bConstructor>
	struct ALLOC_AND_CONSTRUCT 
	{
		template<typename T>
		inline static void free(WvsSingleObjectAllocator* t, T* res)
		{
			static_assert("Unexpected behavior.");
		}
	};

	template<>
	struct ALLOC_AND_CONSTRUCT<false>
	{
		inline static void* alloc(WvsSingleObjectAllocator* t, int nSize)
		{
			return (void*)((unsigned char*)t->ResourceMgr(true) + 4);
		}

		inline static void free(WvsSingleObjectAllocator* t, T* res)
		{
			res = (T*)(((unsigned char*)res) - 4);
			t->ResourceMgr(false, res);
		}
	};

	template<>
	struct ALLOC_AND_CONSTRUCT<true>
	{
		inline static void* alloc(WvsSingleObjectAllocator* t)
		{
			T* res = (T*)((unsigned char*)t->ResourceMgr(true) + 4);
			new(res) T();
			return res;
		}

		inline static void free(WvsSingleObjectAllocator* t, T* res)
		{
			res->~T();
			res = (T*)(((unsigned char*)res) - 4);
			t->ResourceMgr(false, res);
		}
	};

	inline T* Allocate()
	{
		return (T*)ALLOC_AND_CONSTRUCT<!std::is_array<T>::value>::alloc(this);
	}

	inline void Free(void *p)
	{
		ALLOC_AND_CONSTRUCT<!std::is_array<T>::value>::free(this, reinterpret_cast<T*>(p));
	}

	inline static WvsSingleObjectAllocator<T> *GetInstance() 
	{
		static WvsSingleObjectAllocator<T> *p = new WvsSingleObjectAllocator<T>;
		return p;
	}

	template<class... Args>
	inline T* AllocateWithCtor(Args&&... args)
	{
		void *pRet = (void*)(((unsigned char*)ResourceMgr(true, nullptr)) + 4);
		new(pRet) T(std::forward<Args>(args)...);
		return (T*)pRet;
	}
};

//This allocator won't call constructor to initliaze each element.
class WvsArrayAllocator
{
	std::mutex m_mtxLock;

private:
	template<typename T>
	inline void * ResourceMgr(bool allocate = true, int nSize = 1, void *pDel = nullptr)
	{
		std::lock_guard<std::mutex> lock__(m_mtxLock);
		const int N = (int)nSize;
		void *pRet = nullptr;
		if (allocate)
		{
			if (N <= 32)
				pRet = WVS_ALLOCATOR::__gMemPool32.newElement();
			else if (N > 32 && N <= 64)
				pRet = WVS_ALLOCATOR::__gMemPool64.newElement();
			else if (N > 64 && N <= 128)
				pRet = WVS_ALLOCATOR::__gMemPool128.newElement();
			else if (N > 128 && N <= 256)
				pRet = WVS_ALLOCATOR::__gMemPool256.newElement();
			else if (N > 256 && N <= 512)
				pRet = WVS_ALLOCATOR::__gMemPool512.newElement();
			else if (N > 512 && N <= 1024)
				pRet = WVS_ALLOCATOR::__gMemPool1024.newElement();
			else if (N > 1024 && N <= 2048)
				pRet = WVS_ALLOCATOR::__gMemPool2048.newElement();
			else
				pRet = new char[N];

			if (pRet)
			{
				*(int*)pRet = N;
				pRet = (unsigned char*)pRet + 4;
			}
		}
		else
		{
			if (N <= 32)
				WVS_ALLOCATOR::__gMemPool32.deallocate((decltype(WVS_ALLOCATOR::__gMemPool32)::pointer)pDel);
			else if (N > 32 && N <= 64)
				WVS_ALLOCATOR::__gMemPool64.deallocate((decltype(WVS_ALLOCATOR::__gMemPool64)::pointer)pDel);
			else if (N > 64 && N <= 128)
				WVS_ALLOCATOR::__gMemPool128.deallocate((decltype(WVS_ALLOCATOR::__gMemPool128)::pointer)pDel);
			else if (N > 128 && N <= 256)
				WVS_ALLOCATOR::__gMemPool256.deallocate((decltype(WVS_ALLOCATOR::__gMemPool256)::pointer)pDel);
			else if (N > 256 && N <= 512)
				WVS_ALLOCATOR::__gMemPool512.deallocate((decltype(WVS_ALLOCATOR::__gMemPool512)::pointer)pDel);
			else if (N > 512 && N <= 1024)
				WVS_ALLOCATOR::__gMemPool1024.deallocate((decltype(WVS_ALLOCATOR::__gMemPool1024)::pointer)pDel);
			else if (N > 1024 && N <= 2048)
				WVS_ALLOCATOR::__gMemPool2048.deallocate((decltype(WVS_ALLOCATOR::__gMemPool2048)::pointer)pDel);
			else
				::operator delete[] ((unsigned char*)pDel);
		}
		return pRet;
	}

public:

	template<typename T>
	inline void * Allocate(int nSize)
	{
		return ResourceMgr<T>(true, nSize * sizeof(T) + 4);
	}

	template<typename T>
	inline void Free(void *p)
	{
		p = ((unsigned char*)p) - 4;
		ResourceMgr<T>(false, *(int*)p, p);
	}

	inline static WvsArrayAllocator * GetInstance() 
	{
		static WvsArrayAllocator *p = new WvsArrayAllocator;
		return p;
	}
};

#pragma warning(default:4624)  
#endif // MEMORY_POOL_H
