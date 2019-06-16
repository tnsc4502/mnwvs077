#pragma once
#include "..\WvsLib\Memory\ZMemory.h"
struct GW_ItemSlotBase;

struct ExchangeElement
{
	int m_nItemID = 0, m_nCount = 0;
	ZSharedPtr<GW_ItemSlotBase> m_pItem;
};

