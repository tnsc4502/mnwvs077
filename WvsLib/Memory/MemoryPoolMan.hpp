#pragma once
#include "MemoryPool.h"

#define AllocObj(instance_type) WvsSingleObjectAllocator<instance_type>::GetInstance()->Allocate()
#define AllocObjCtor(instance_type) WvsSingleObjectAllocator<instance_type>::GetInstance()->AllocateWithCtor
#define FreeObj(pointer) WvsSingleObjectAllocator<std::remove_reference<decltype(*pointer)>::type>::GetInstance()->Free(pointer)

#define AllocArray(instance_type, nSize) (instance_type*)WvsArrayAllocator::GetInstance()->Allocate<instance_type>(nSize)
#define FreeArray(pointer) WvsArrayAllocator::GetInstance()->Free<char>(pointer) //type free
