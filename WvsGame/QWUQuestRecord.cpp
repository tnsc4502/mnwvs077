#include "QWUQuestRecord.h"
#include "User.h"
#include "QuestMan.h"
#include "QuestDemand.h"
#include <algorithm>
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_QuestRecord.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include <mutex>


int QWUQuestRecord::GetState(User * pUser, int nKey)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto pCharacterData = pUser->GetCharacterData();
	auto findIter = pCharacterData->mQuestRecord.find(nKey);
	if (findIter != pCharacterData->mQuestRecord.end())
		return findIter->second->nState;

	findIter = pCharacterData->mQuestComplete.find(nKey);
	if (findIter != pCharacterData->mQuestComplete.end())
		return findIter->second->nState;

	return 0;
}

void QWUQuestRecord::Remove(User * pUser, int nKey, bool bComplete)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto pCharacterData = pUser->GetCharacterData();
	if (bComplete)
		pCharacterData->mQuestComplete.erase(nKey);
	else
		pCharacterData->RemoveQuest(nKey);
	pUser->SendQuestRecordMessage(nKey, 0, "");
}

std::string QWUQuestRecord::Get(User * pUser, int nKey)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto pCharacterData = pUser->GetCharacterData();
	auto findIter = pCharacterData->mQuestRecord.find(nKey);
	if (findIter != pCharacterData->mQuestRecord.end())
		return findIter->second->sStringRecord;
	return "";
}

void QWUQuestRecord::Set(User *pUser, int nKey, const std::string &sInfo)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto pCharacter = pUser->GetCharacterData();
	pCharacter->SetQuest(nKey, sInfo);

	auto pDemand = QuestMan::GetInstance()->GetCompleteDemand(nKey);
	if (pDemand && pDemand->m_mDemandMob.size() > 0)
	{
		std::string sInfoSet = "";
		auto pRecord = pCharacter->mQuestRecord[nKey];
		for (auto& prMob : pDemand->m_mDemandMob)
		{
			pRecord->aMobRecord.push_back(0);
			sInfoSet += "000";
		}
		pRecord->sStringRecord = sInfoSet;
	}
	
	pUser->SendQuestRecordMessage(nKey, 1, sInfo);
}

void QWUQuestRecord::SetMobRecord(User *pUser, int nKey, int nMobTempleteID)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto pCharacterData = pUser->GetCharacterData();
	auto findIter = pCharacterData->mQuestRecord.find(nKey);
	if (findIter == pCharacterData->mQuestRecord.end() || findIter->second->aMobRecord.size() == 0)
		return;

	auto pRecord = findIter->second;
	auto pDemand = QuestMan::GetInstance()->GetCompleteDemand(nKey);
	if (pDemand && pDemand->m_aDemandMob.size() > 0)
	{
		int nDemand = pDemand->m_mDemandMob[nMobTempleteID];
		for (int i = 0; i < pDemand->m_aDemandMob.size(); ++i)
			if (pDemand->m_aDemandMob[i] == nMobTempleteID)
			{
				pRecord->aMobRecord[i] = std::min(nDemand, pRecord->aMobRecord[i] + 1);
				break;
			}
		
		pRecord->sStringRecord = "";
		char aBuffer[4]{ 0 };
		for (int i = 0; i < (int)pRecord->aMobRecord.size(); ++i)
		{
			sprintf_s(aBuffer, "%03d", pRecord->aMobRecord[i]);
			pRecord->sStringRecord += aBuffer;
		}
		pUser->SendQuestRecordMessage(nKey, 1, pRecord->sStringRecord);
	}
}

void QWUQuestRecord::ValidateMobCountRecord(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	for (auto& prRecord : pUser->GetCharacterData()->mQuestRecord)
	{
		auto& aDemandMob = QuestMan::GetInstance()->GetCompleteDemand(prRecord.first)->m_aDemandMob;
		if (aDemandMob.size() > 0)
		{
			if (prRecord.second->sStringRecord.size() != aDemandMob.size() * 3)
				prRecord.second->sStringRecord = std::string(aDemandMob.size() * 3, '0');

			prRecord.second->aMobRecord.resize(aDemandMob.size());
			for (int i = 0; i < (int)aDemandMob.size(); ++i)
				sscanf_s(prRecord.second->sStringRecord.c_str() + (i * 3), "%03d", &(prRecord.second->aMobRecord[i]));

			pUser->SendQuestRecordMessage(prRecord.first, 1, prRecord.second->sStringRecord);
		}
	}
}

void QWUQuestRecord::SetComplete(User * pUser, int nKey)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto pCharacterData = pUser->GetCharacterData();
	pCharacterData->RemoveQuest(nKey);
	GW_QuestRecord *pComplete = AllocObj(GW_QuestRecord);
	pComplete->nQuestID = nKey;
	pComplete->nState = 2;
	pComplete->tTime = GameDateTime::GetTime();
	pCharacterData->mQuestComplete.insert({ nKey , pComplete });
	pUser->SendQuestRecordMessage(nKey, 2, "");
}
