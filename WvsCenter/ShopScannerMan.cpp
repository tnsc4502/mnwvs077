#include "ShopScannerMan.h"
#include "EntrustedShopMan.h"
#include "CenterPacketTypes.hpp"

#include "..\Database\GW_ItemSlotBase.h"
#include "..\Database\EntrustedShopDBAccessor.h"

#include "..\WvsLib\Memory\ZMemory.h"
#include "..\WvsLib\Net\SocketBase.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Logger\WvsLogger.h"

#include <algorithm>

ShopScannerMan * ShopScannerMan::GetInstance()
{
	static ShopScannerMan* pInstance = new ShopScannerMan;
	return pInstance;
}

void ShopScannerMan::Search(SocketBase * pSrv, int nCharacterID, int nWorldID, int nItemID, int nPOS)
{
	std::lock_guard<std::recursive_mutex> lock(EntrustedShopMan::GetInstance()->GetLock());
	int nTI = nItemID / 1000000;
	auto aRet = EntrustedShopDBAccessor::QueryItemExistence(nWorldID, nItemID);
	OutPacket oPacket;
	oPacket.Encode2(CenterResultPacketType::ShopScannerResult);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode2(aRet.size() ? nPOS : 0);
	oPacket.Encode4(nItemID);

	int nCountPos = oPacket.GetPacketSize(), nInvalidCount = 0;
	oPacket.Encode4((int)aRet.size());

	for (auto& prRes : aRet)
	{
		auto pShop = EntrustedShopMan::GetInstance()->GetShopData(prRes.first);
		const EntrustedShopMan::ShopItem* pShopItem = nullptr;
		if (!pShop || (pShopItem = pShop->GetShopItem(prRes.second)) == nullptr)
		{
			++nInvalidCount;
			continue;
		}
		oPacket.EncodeStr(pShop->sEmployer);
		oPacket.Encode4(pShop->nFieldID);
		oPacket.EncodeStr(pShop->sTitle);
		oPacket.Encode4(pShopItem->nNumber);
		oPacket.Encode4(pShopItem->nSet); 
		oPacket.Encode4(pShopItem->nPrice);
		oPacket.Encode4(pShop->nShopSN);
		oPacket.Encode1(pShop->nChannelID);
		oPacket.Encode1(nTI);
		if (nTI == 1)
			pShopItem->pItem->RawEncode(&oPacket);
	}

	if (nInvalidCount)
		*((int*)(oPacket.GetPacket() + nCountPos)) = std::max(0, (int)aRet.size() - nInvalidCount);

	pSrv->SendPacket(&oPacket);
}
