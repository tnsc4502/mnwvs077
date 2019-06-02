#include "WvsUnified.h"
#include "Poco\Data\Data.h"
#include "Poco\Data\Statement.h"
#include "DBConstants.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "Poco\Data\MySQL\Connector.h"

ConfigLoader *WvsUnified::ms_pCfg = nullptr;

WvsUnified::WvsUnified()
	: mDBSessionPool((Poco::Data::MySQL::Connector::registerConnector(), Poco::Data::MySQL::Connector::KEY),
		"host=" + ms_pCfg->StrValue("DB_Host") +
		";user=" + ms_pCfg->StrValue("DB_User") +
		";password=" + ms_pCfg->StrValue("DB_Pass") +
		";db=" + ms_pCfg->StrValue("DB_Name") +
		";character-set=big5;auto-reconnect=true")
{
	if (!mDBSessionPool.get().isConnected())
	{
		printf("WvsUnified Init Failed.\n");
		throw std::runtime_error("WvsUnified Init Failed.");
	}
}

WvsUnified::~WvsUnified()
{
}

void WvsUnified::InitDB(ConfigLoader* pCfg)
{
	ms_pCfg = pCfg;
}

WvsUnified* WvsUnified::GetInstance()
{
	static WvsUnified* pInstance = new WvsUnified();
	return pInstance;
}

Poco::Data::Session WvsUnified::GetDBSession()
{
	return (mDBSessionPool.get());
}
