#include "FieldMan.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "TimerThread.h"

#include "Field.h"
#include "Field_MonsterCarnival.h"
#include "FieldSet.h"
#include "PortalMap.h"
#include "ReactorPool.h"
#include "WvsPhysicalSpace2D.h"

#include <mutex>
#include <filesystem>
#include <fstream>
#include <streambuf>

std::mutex fieldManMutex;

namespace fs = std::experimental::filesystem;

FieldMan::FieldMan()
{
}

FieldMan::~FieldMan()
{
}

FieldMan * FieldMan::GetInstance()
{
	static FieldMan *sPtrFieldMan = new FieldMan();
	return sPtrFieldMan;
}

void FieldMan::RegisterField(int nFieldID)
{
	std::lock_guard<std::mutex> guard(fieldManMutex);
	FieldFactory(nFieldID);
}

void FieldMan::FieldFactory(int nFieldID)
{
	/*if (mField[nFieldID]->GetFieldID() != 0)
		return;*/
		/*
		在這裡檢查此Field的型態，舊版的在這裡根據FieldType建立不同的地圖實體
		Field
		Field_Tutorial
		Field_ShowaBath
		Field_WeddingPhoto
		Field_SnowBall
		Field_Tournament
		Field_Coconut
		Field_OXQuiz
		Field_PersonalTimeLimit
		Field_GuildBoss
		Field_MonsterCarnival
		Field_Wedding
		...
		*/
	std::string sField = std::to_string(nFieldID);
	while (sField.size() < 9)
		sField = "0" + sField;
	auto& mapWz = stWzResMan->GetWz(Wz::Map)["Map"]["Map" + std::to_string(nFieldID / 100000000)][sField];
	if (mapWz == WZ::Node())
		return;

	auto& infoData = mapWz["info"];
	int nFieldType = infoData["fieldType"];

	Field* pField = nullptr;
	switch (nFieldType)
	{
		case 10:
			pField = AllocObjCtor(Field_MonsterCarnival)(&mapWz, nFieldID);
			break;
		default:
			pField = AllocObjCtor(Field)(&mapWz, nFieldID);
			break;
	}

	RestoreFoothold(pField, &(mapWz["foothold"]), nullptr, &infoData);
	pField->InitLifePool();
	m_mField[nFieldID] = pField;
	TimerThread::RegisterField(pField);
}

void FieldMan::LoadFieldSet()
{
	std::string strPath = "./DataSrv/FieldSet";
	for (auto &file : fs::directory_iterator(strPath))
	{
		FieldSet *pFieldSet = AllocObj( FieldSet );
		std::wstring wStr = file.path();
		//Convert std::wstring to std::string, note that the path shouldn't include any NON-ASCII character.
		pFieldSet->Init(std::string{ wStr.begin(), wStr.end() });
		m_mFieldSet[pFieldSet->GetFieldSetName()] = pFieldSet;
	}
}

Field* FieldMan::GetField(int nFieldID)
{
	auto fieldResult = m_mField.find(nFieldID);
	if (fieldResult == m_mField.end())
		RegisterField(nFieldID);
	return m_mField[nFieldID];
}

FieldSet * FieldMan::GetFieldSet(const std::string & sFieldSetName)
{
	auto fieldResult = m_mFieldSet.find(sFieldSetName);
	if (fieldResult == m_mFieldSet.end())
		return nullptr;
	return fieldResult->second;
}

void FieldMan::RestoreFoothold(Field * pField, void * pPropFoothold, void * pLadderOrRope, void * pInfo)
{
	auto& refInfo = *((WZ::Node*)pInfo);
	int nFieldLink = (nFieldLink = atoi(((std::string)refInfo["link"]).c_str()));
	if ((nFieldLink != 0))
	{
		auto fieldStr = StringUtility::LeftPadding(std::to_string(nFieldLink), 9, '0');
		auto& mapWz = stWzResMan->GetWz(Wz::Map)["Map"]["Map" + std::to_string(nFieldLink / 100000000)][fieldStr];
		pInfo = &(mapWz["info"]);
		pPropFoothold = &(mapWz["foothold"]);
	}
	pField->GetSpace2D()->Load(pPropFoothold, pLadderOrRope, pInfo);
	pField->SetMapSize(
		pField->GetSpace2D()->GetRect().left,
		pField->GetSpace2D()->GetRect().top
	);
	pField->SetLeftTop(
		pField->GetSpace2D()->GetRect().right - pField->GetSpace2D()->GetRect().left,
		pField->GetSpace2D()->GetRect().bottom - pField->GetSpace2D()->GetRect().top
	);
}
