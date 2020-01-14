#pragma once
#include <string>
#include <map>
#include "..\Memory\MemoryPool.h"
#include "..\Common\CommonDef.h"
#include "WzNameSpace.h"
#include "WzDelayedVariant.h"

class WzArchive;

class WzProperty : public WzNameSpace
{
	ALLOW_PRIVATE_ALLOC
public:
	typedef std::map<std::string, WzProperty*> PropContainerType;

private:
	unsigned int m_uRootPropPos = 0;
	WzDelayedVariant m_wzVariant;

	//Variant property
	WzProperty(const std::string& sPropName);

	void DelayParse();
public:
	WzProperty(WzArchive *pArchive, const std::string& sPropName, unsigned int uBeginPos, unsigned int uRootPropPos);
	~WzProperty();

	void OnGetItem();
	WzDelayedVariant& GetVariant();
	WzProperty* GetProperty();
};


template<>
class WvsSingleObjectAllocator<WzProperty>
{
	MemoryPool<WzProperty> m_Pool;

public:
	inline void* ResourceMgr(bool bAllocate = true, void *pDel = nullptr)
	{
		if (bAllocate)
			return m_Pool.allocate();
		else
			m_Pool.deallocate((WzProperty*)pDel);

		return nullptr;
	}

public:
	WvsSingleObjectAllocator() 
		: m_Pool(sizeof(WzProperty), sizeof(WzProperty) * 128) {}

	template<class... Args>
	inline WzProperty* AllocateWithCtor(Args&&... args)
	{
		void *pRet = (void*)(((unsigned char*)ResourceMgr(true, nullptr)) + 2);
		new(pRet) WzProperty(std::forward<Args>(args)...);
		return (WzProperty*)pRet;
	}

	inline void Free(void *p)
	{
		((WzProperty*)(p))->~WzProperty();
		ResourceMgr(false, ((unsigned char*)p) - 2);
	}

	inline static WvsSingleObjectAllocator<WzProperty> *GetInstance()
	{
		static WvsSingleObjectAllocator<WzProperty> *p = new WvsSingleObjectAllocator<WzProperty>;
		return p;
	}

	inline void Release()
	{
		m_Pool.release();
	}
};