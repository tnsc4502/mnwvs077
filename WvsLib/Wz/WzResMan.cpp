#include "WzResMan.hpp"
#include "..\Memory\MemoryPoolMan.hpp"

WzResMan *stWzResMan = WzResMan::GetInstance();

void WzResMan::RemountAll()
{
	m_FileSystem.UnmountAll();
	WvsSingleObjectAllocator<WzProperty>::GetInstance()->Release();
	Init();
}
