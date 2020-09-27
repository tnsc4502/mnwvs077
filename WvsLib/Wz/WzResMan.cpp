#include "WzResMan.hpp"
#include "..\Memory\MemoryPoolMan.hpp"

void WzResMan::RemountAll()
{
	m_FileSystem.UnmountAll();
	//WvsSingleObjectAllocator<WzProperty>::GetInstance()->Release();
	Init();
}
