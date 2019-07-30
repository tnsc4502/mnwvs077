#pragma once
#include "MemoryPoolMan.hpp"

template<typename T>
static void ZRelease(void *p)
{
	if (p)
		WvsSingleObjectAllocator<T>::GetInstance()->Free(p);
}

static void ZReleaseArray(void *p)
{
	if (p)
		WvsArrayAllocator::GetInstance()->Free<char>(p);
}

template<typename T>
static T* ZAllocate()
{
	return WvsSingleObjectAllocator<T>::GetInstance()->Allocate();
}

template<typename T>
static T* ZAllocateArray(int nSize)
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
	const T& operator *() const { return *(m_p); }
	const T* operator ->() const { return m_p; }
	operator bool() const { return m_p ? true : false; }
	bool operator == (void *p) const { return m_p == p; }
	bool operator != (void *p) const { return m_p != p; }
	template<typename U>
	operator U*() { return (U*)m_p; }

	template<typename U>
	operator U*() const { return (U*)m_p; }

	void swap(ZUniquePtr& lv) { std::swap(m_p, lv.m_p); }
	void reset(ZUniquePtr&& rv) { m_p = rv.m_p; rv.m_p = nullptr; }
	void reset(T *p) { ZRelease<T>(m_p); m_p = p; }

	~ZUniquePtr() { ZRelease<T>(m_p); }
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
	operator bool() { return m_p ? true : false; }

	template<typename U>
	operator U*() { return (U*)m_p; }

	template<typename U>
	operator U*() const { return (U*)m_p; }

	void reset(T *p) { ZReleaseArray(m_p); m_p = p; }
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
		(T*)WvsArrayAllocator::GetInstance()->Allocate<T>(nSize)
	);
}

template<typename T>
class ZSharedPtr
{
	template <typename U>
	friend class ZSharedPtr;

	std::shared_ptr<T> m_s;
	
	template <typename U>
	ZSharedPtr& operator =(U* p) = delete; //Use reset instead.

public:
	ZSharedPtr() {}
	ZSharedPtr(T *p) { m_s.reset(p, ZRelease<T>); }

	template<typename U>
	ZSharedPtr(const ZSharedPtr<U>& lhs) { m_s = lhs.m_s; }

	T& operator *() { return *(m_s.get()); }
	T* operator ->() { return m_s.get(); }
	const T& operator *() const  { return *(m_s.get()); }
	const T* operator ->() const { return m_s.get(); }
	operator bool() const { return m_s.get() ? true : false; }

	template<typename SP>
	bool operator == (const SP& p) const { return m_s == p; }

	template<typename SP>
	bool operator != (const SP& p) const { return m_s != p; }

	template<typename U>
	operator U*() { return (U*)m_s.get(); }

	template<typename U>
	operator U*() const { return (U*)m_s.get(); }

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
	operator T*() { return m_s.get(); }

	template<typename SP>
	bool operator == (const SP& p) const { return m_s == p; }

	template<typename SP>
	bool operator != (const SP& p) const { return m_s != p; }

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