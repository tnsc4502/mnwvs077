#pragma once

#include <string>
#include <vector>
#include "Poco\Data\Session.h"
#include "Poco\Data\SessionPool.h"
#include "Poco\Data\RecordSet.h"

#define GET_DB_SESSION WvsUnified::GetInstance()->GetDBSession()
class ConfigLoader;

class WvsUnified
{
private:
	Poco::Data::SessionPool mDBSessionPool;
	static ConfigLoader *ms_pCfg;

public:
	typedef Poco::Data::RecordSet ResultType;
	WvsUnified();
	~WvsUnified();
	static void InitDB(ConfigLoader *pCfg);
	static WvsUnified* GetInstance();
	Poco::Data::Session GetDBSession();
};