#pragma once
#include "MemoryPoolMan.hpp"

template<typename T>
void ZRelease(void *p)
{
	if (p)
		WvsSingleObjectAllocator<T>::GetInstance()->Free(p);
}

void ZReleaseArray(void *p)
{
	if (p)
		WvsArrayAllocator::GetInstance()->Free<unsigned char>(p);
}

template<typename T>
T* ZAllocate()
{
	return WvsSingleObjectAllocator<T>::GetInstance()->Allocate();
}

template<typename T>
T* ZAllocateArray(int nSize)
{
	return (T*)WvsArrayAllocator::GetInstance()->Allocate<T>(nSize);
}

template<typename T>
class ZUniquePtr
{
	T* m_p  = nullptr;
	ZUniquePtr& operator =(ZUniquePtr& z) = delete;
	ZUniquePtr& operator =(T* p) = delete;

public:
	ZUniquePtr() {}
	ZUniquePtr(T *p) { m_p = p; }
	ZUniquePtr(ZUniquePtr&& rv) { m_p = rv.m_p; rv.m_p = nullptr; }
	T& operator *() { return *m_p; }
	T* operator ->() { return m_p; }

	~ZUniquePtr() { ZRelease(m_p); }
};

template<typename T>
class ZUniquePtr<T[]>
{
	T* m_p = nullptr;
	ZUniquePtr& operator =(ZUniquePtr& z) = delete;
	ZUniquePtr& operator =(T* p) = delete;

public:
	ZUniquePtr() {}
	ZUniquePtr(T *p) { m_p = p; }
	ZUniquePtr(ZUniquePtr&& rv) { m_p = rv.m_p; rv.m_p = nullptr; }
	T* operator +(int nOffset) { return m_p + nOffset; }
	T& operator *() { return *m_p; }
	T& operator[](int nIdx) { return m_p[nIdx]; }

	~ZUniquePtr() { ZReleaseArray(m_p); }
};

template<typename T, class ...Args>
ZUniquePtr<T> MakeUnique(Args&&... args)
{
	return ZUniquePtr<T>(
		WvsSingleObjectAllocator<T>::GetInstance()->AllocateWithCtor(std::forward<Args>(args)...)
		);
}

template<typename T>
ZUniquePtr<T[]> MakeUniqueArray(int nSize)
{
	return ZUniquePtr<T[]>(
		WvsArrayAllocator::GetInstance()->Allocate<T>(nSize)
	);
}

template<typename T>
class ZSharedPtr
{
	template <typename U>
	friend class ZSharedPtr;

	std::shared_ptr<T> m_s;

public:
	ZSharedPtr() {}
	ZSharedPtr(T *p) { m_s.reset(p, ZRelease<T>); }

	template<typename U>
	ZSharedPtr(const ZSharedPtr<U>& lhs) { m_s = lhs.m_s; }

	T& operator *() { return *(m_s.get()); }
	T* operator ->() { return m_s.get(); }
	operator bool() { return m_s.get() ? true : false; }
	void reset(T *p) { m_s.reset(p, ZRelease<T>); }

};

template<typename T>
class ZSharedPtr<T[]>
{
	std::shared_ptr<T> m_s;

public:
	ZSharedPtr() {}
	ZSharedPtr(T *p) { m_s.reset(p, ZReleaseArray); }
	ZSharedPtr(const ZSharedPtr& lhs) { m_s = lhs.m_s; }

	T* operator +(int nOffset) { return m_s.get() + nOffset; }
	T& operator *() { return *(m_s.get()); }
	T& operator[](int nIdx) { return m_s.get()[nIdx]; }
	operator bool() { return m_s.get() ? true : false; }
	void reset(T *p) { m_s.reset(p, ZReleaseArray); }
};

template<typename T, class ...Args>
ZSharedPtr<T> MakeShared(Args&&... args)
{
	return ZSharedPtr<T>(
		WvsSingleObjectAllocator<T>::GetInstance()->AllocateWithCtor(std::forward<Args>(args)...)
		);
}

template<typename T>
ZSharedPtr<T[]> MakeSharedArray(int nSize)
{
	return ZSharedPtr<T[]>(
		(T*)WvsArrayAllocator::GetInstance()->Allocate<T>(nSize)
		);
}