#pragma once

class SocketBase;

class ShopScannerMan
{
public:
	enum ShopScannerRequestType
	{
		eScanner_OnLoadPopularList,
		eScanner_OnSearch
	};

	static ShopScannerMan* GetInstance();
	void Search(SocketBase *pSrv, int nCharacterID, int nWorldID, int nItemID, int nPOS);
};

