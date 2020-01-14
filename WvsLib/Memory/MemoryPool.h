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
	const int UnitSize;
	//   typedef std::false_type propagate_on_container_copy_assignment;
	//   typedef std::true_type  propagate_on_container_move_assignment;
	//   typedef std::true_type  propagate_on_container_swap;

	template <typename U> struct rebind {
		typedef MemoryPool<U> other;
	};

	size_t BlockSize;

	/* Member functions */
	MemoryPool(int nUnitSize, int nBlockSize) noexcept;
	MemoryPool(int nBlockSize) noexcept;
	MemoryPool(const MemoryPool& memoryPool) noexcept;
	MemoryPool(MemoryPool&& memoryPool) noexcept;
	template <class U> MemoryPool(const MemoryPool<U>& memoryPool) noexcept;

	~MemoryPool() noexcept;
	void release() noexcept;

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
	void allocateBlock();

private:
	union Slot_ {
		//value_type element;
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

	//static_assert(BlockSize >= 2 * sizeof(slot_type_), "BlockSize too small.");
};

#include "MemoryPool.tcc"
#include <type_traits>

constexpr size_t GetAlignedPoolPos(const size_t nAlloc)
{
	return ((nAlloc < 2) ? 0 : 1 + GetAlignedPoolPos(nAlloc / 2));
}

constexpr size_t Pow2(const size_t nPower)
{
	return nPower == 0 ? 1 : 2 * Pow2(nPower - 1);
}

class MemoryPoolMan
{
public:
	static const size_t MAX_ALLOC_SIZE = 8192 * 8;
	static const size_t INITIAL_BLOCK_NUM = 128;
	static const size_t MAX_POOL_NUM = GetAlignedPoolPos(MAX_ALLOC_SIZE) + 1; // extra one for 0~1 byte

private:
	MemoryPool<char>* m_apPool[MAX_POOL_NUM];
	MemoryPool<char>* m_apArrayPool[MAX_POOL_NUM];
	MemoryPoolMan();

public:
	static MemoryPoolMan* GetInstance() 
	{
		static MemoryPoolMan* pInstance = new MemoryPoolMan;
		return pInstance;
	}

	MemoryPool<char>* GetPool(size_t nPos);
	MemoryPool<char>* GetArrayPool(size_t nPos);
};

template<typename T>
class WvsSingleObjectAllocator
{
public:
	inline void* ResourceMgr(bool bAllocate = true, void *pDel = nullptr)
	{
		void *pRet = nullptr;
		if (bAllocate)
		{
			const size_t nAllocSize = sizeof(T) + 2;
			const size_t nAlignPos = GetAlignedPoolPos(nAllocSize);
			const size_t nAllocPos = (nAllocSize > Pow2(nAlignPos) ? nAlignPos + 1 : nAlignPos);

			if (nAllocSize > MemoryPoolMan::MAX_ALLOC_SIZE)
			{
				pRet = new char[nAllocSize];
				*((unsigned char*)pRet) = 0xFF;
				return pRet;
			}
			else
				pRet = MemoryPoolMan::GetInstance()->GetPool(nAllocPos)->allocate();

			if (pRet)
				*((unsigned char*)pRet) = (unsigned char)nAllocPos;
		}
		else
		{
			size_t nAllocPos = *(unsigned char*)pDel;
			if (nAllocPos == 0xFF)
			{
				delete[] pDel;
				return nullptr;
			}

			MemoryPoolMan::GetInstance()->GetPool(nAllocPos)->deallocate((char*)pDel);
		}
		return pRet;
	}

public:
	template<class... Args>
	inline T* AllocateWithCtor(Args&&... args)
	{
		void *pRet = (void*)(((unsigned char*)ResourceMgr(true, nullptr)) + 2);
		new(pRet) T(std::forward<Args>(args)...);
		return (T*)pRet;
	}

	inline T* Allocate()
	{
		T* res = (T*)((unsigned char*)ResourceMgr(true) + 2);
		new(res) T();
		return res;
	}

	inline void Free(void *p)
	{
		((T*)(p))->~T();
		ResourceMgr(false, ((unsigned char*)p) - 2);
	}

	inline static WvsSingleObjectAllocator<T> *GetInstance() 
	{
		static WvsSingleObjectAllocator<T> *p = new WvsSingleObjectAllocator<T>;
		return p;
	}
};

//This allocator won't call constructor to initliaze each element.
class WvsArrayAllocator
{
private:
	template<typename T>
	inline void * ResourceMgr(bool bAllocate = true, int nSize = 1, void *pDel = nullptr)
	{
		void *pRet = nullptr;
		if (bAllocate)
		{
			const size_t nAllocSize = nSize + 2;
			const size_t nAlignPos = GetAlignedPoolPos(nAllocSize);
			const size_t nAllocPos = (nAllocSize > Pow2(nAlignPos) ? nAlignPos + 1 : nAlignPos);

			if (nAllocSize > MemoryPoolMan::MAX_ALLOC_SIZE)
			{
				pRet = new char[nAllocSize];
				*((unsigned char*)pRet) = 0xFF;
				return pRet;
			}
			else
				pRet = MemoryPoolMan::GetInstance()->GetArrayPool(nAllocPos)->allocate();

			if (pRet)
				*((unsigned char*)pRet) = (unsigned char)nAllocPos;
		}
		else
		{
			size_t nAllocPos = *(unsigned char*)pDel;
			if (nAllocPos == 0xFF)
			{
				delete[] pDel;
				return nullptr;
			}

			MemoryPoolMan::GetInstance()->GetArrayPool(nAllocPos)->deallocate((char*)pDel);
		}
		return pRet;
	}

public:
	template<typename T>
	inline void* Allocate(int nSize)
	{
		return (unsigned char*)ResourceMgr<T>(true, nSize * sizeof(T)) + 2;
	}

	template<typename T>
	inline void Free(void *p)
	{
		p = ((unsigned char*)p) - 2;
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
