#pragma once
#include "..\WvsLib\Memory\ZMemory.h"

struct GW_ItemSlotBase;

struct BackupItem
{
	int m_nTI, m_nPOS;
	ZSharedPtr<GW_ItemSlotBase> m_pItem;
	bool bClone;
};

