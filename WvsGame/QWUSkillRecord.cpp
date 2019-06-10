#include "QWUSkillRecord.h"
#include "User.h"
#include "SkillInfo.h"
#include "SkillEntry.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_SkillRecord.h"
#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

QWUSkillRecord::QWUSkillRecord()
{
}


QWUSkillRecord::~QWUSkillRecord()
{
}

void QWUSkillRecord::GetSkillRootFromJob(int nJob, std::vector<int>& aRet)
{
	int nJobType = nJob / 100, nJobRoot = 0, nJobAdv = 0;
	if (nJobType)
	{
		nJobRoot = nJobType * 100;
		aRet.push_back(nJobRoot);
		nJobAdv = nJob % 100 / 10;
		if (nJobAdv)
		{
			aRet.push_back(nJobRoot + 10 * nJobAdv);
			if (nJob % 10)
			{
				aRet.push_back(nJobRoot + 10 * nJobAdv + 1);
				if(nJob % 10 >= 2)
					aRet.push_back(nJobRoot + 10 * nJobAdv + 2);
			}
		}
	}
}

bool QWUSkillRecord::SkillUp(User * pUser, int nSkillID, int nAmount, bool bDecSP, bool bCheckMasterLevel, std::vector<GW_SkillRecord*>& aChange)
{
	int nJob = pUser->GetCharacterData()->mStat->nJob;
	int nSkillJob = WvsGameConstants::GetSkillRootFromSkill(nSkillID);

	if ((nJob >= nSkillJob) &&
		((nJob < 10000 && (nJob / 100 == nSkillJob / 100)) ||
		(nJob / 1000 == nSkillJob / 1000)))
	{
		int nSkillRootLevel = WvsGameConstants::GetJobLevel(nSkillJob);
		if (/*nSkillRootLevel >= 0 &&
			nSkillRootLevel < GW_CharacterStat::EXTEND_SP_SIZE &&*/
			(!bDecSP || pUser->GetCharacterData()->mStat->aSP[0] >= nAmount))
		{
			auto pSkillRecord = pUser->GetCharacterData()->GetSkill(nSkillID);
			if (pSkillRecord == nullptr)
			{
				pSkillRecord = SkillInfo::GetInstance()->GetSkillRecord(
					nSkillID,
					0,
					0
				);
				pUser->GetCharacterData()->ObtainSkillRecord(pSkillRecord);
			}
			if (pSkillRecord != nullptr 
				&& (!bCheckMasterLevel 
					|| (!WvsGameConstants::IsSkillNeedMasterLevel(pSkillRecord->nSkillID)
						|| (pSkillRecord->nSLV + nAmount <= pSkillRecord->nMasterLevel))
					)
				)
			{
				pSkillRecord->nSLV += nAmount;
				if(bDecSP)
					pUser->GetCharacterData()->mStat->aSP[0] -= nAmount;
				aChange.push_back(pSkillRecord);
				return true;
			}
		}
	}
	return false;
}

void QWUSkillRecord::SendCharacterSkillRecord(User * pUser, std::vector<GW_SkillRecord*>& aChange)
{
	WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "SendCharacterSkillRecord Called, size = %d\n", (int)aChange.size());
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnChangeSkillRecordResult);
	oPacket.Encode1(1);
	oPacket.Encode2((short)aChange.size());
	for (auto pSkillRecord : aChange)
	{
		WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "SendCharacterSkillRecord Skill ID = %d SLV = %d MasterLevel = %d\n", pSkillRecord->nSkillID, pSkillRecord->nSLV, pSkillRecord->nMasterLevel);
		oPacket.Encode4(pSkillRecord->nSkillID);
		oPacket.Encode4(pSkillRecord->nSLV);
		oPacket.Encode4(pSkillRecord->nMasterLevel);
		//oPacket.Encode8(pSkillRecord->tExpired);
	}
	oPacket.Encode4(0);
	pUser->SendPacket(&oPacket);
}

bool QWUSkillRecord::MasterLevelChange(User *pUser, int nSkillID, int nMasterLevel)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	std::vector<GW_SkillRecord*> aChange;
	GW_SkillRecord* pRecord = nullptr;
	if (pUser->GetCharacterData()->mStat->nHP &&
		nSkillID / 10000 % 100 &&
		nSkillID / 10000 % 10 == 2 &&
		(pRecord = pUser->GetCharacterData()->GetSkill(nSkillID)))
	{
		pRecord->nMasterLevel = nMasterLevel;
		aChange.push_back(pRecord);
		pUser->ValidateStat();
		SendCharacterSkillRecord(pUser, aChange);
		return true;
	}
	return false;
}

bool QWUSkillRecord::MakeSkillVisible(User *pUser, int nSkillID, int nSkillLevel, int nMasterLevel)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto cd = pUser->GetCharacterData();
	if (cd->mStat->nHP == 0)
		return false;

	std::vector<int> aJob;
	int nSkillJob = nSkillID / 10000, nJobIndex = -1;
	GetSkillRootFromJob(cd->mStat->nJob, aJob);
	for(int i = 0; i < (int)aJob.size(); ++i)
		if (aJob[i] == nSkillJob)
		{
			nJobIndex = i;
			break;
		}
	if (aJob.size() == 0 || nJobIndex == -1)
		return false;

	auto pEntry = SkillInfo::GetInstance()->GetSkillByID(nSkillID);
	if (!pEntry ||
		!pEntry->IsInvisible() ||
		cd->GetSkill(nSkillID))
		return false;

	if (nSkillJob % 100 && nSkillJob % 10 == 2)
		if (nMasterLevel == -1) 
		{
			if (nJobIndex >= 0)
				nMasterLevel = pEntry->GetMasterLevel();
			else
				nMasterLevel = 0;
		}

	auto pRecord = AllocObj(GW_SkillRecord);
	pRecord->nSkillID = nSkillID;
	pRecord->nSLV = 0;
	pRecord->nMasterLevel = nMasterLevel;
	cd->mSkillRecord.insert({ nSkillID, pRecord });
	pUser->ValidateStat();
	SendCharacterSkillRecord(pUser, std::vector<GW_SkillRecord*>{ pRecord });
	return true;
}

void QWUSkillRecord::ValidateMasterLevelForSKills(User *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto cd = pUser->GetCharacterData();
	std::vector<GW_SkillRecord*> aChange;
	std::vector<int> aJob;
	GetSkillRootFromJob(cd->mStat->nJob, aJob);
	GW_SkillRecord* pRecord = nullptr;
	for (auto nJob : aJob)
	{
		auto pSkills = SkillInfo::GetInstance()->GetSkillsByRootID(nJob);
		if (!pSkills)
			continue;
		for (auto& prEntry : *pSkills)
		{
			if (prEntry.second->IsInvisible() ||
				!prEntry.second->GetMasterLevel())
				continue;

			pRecord = cd->GetSkill(prEntry.second->GetSkillID());
			if (!pRecord) 
			{
				pRecord = AllocObj(GW_SkillRecord);
				pRecord->nCharacterID = pUser->GetUserID();
				cd->mSkillRecord.insert({ prEntry.second->GetSkillID(), pRecord });
			}
			pRecord->nSkillID = prEntry.second->GetSkillID();
			pRecord->nMasterLevel = std::max(pRecord->nMasterLevel, prEntry.second->GetMasterLevel());
			aChange.push_back(pRecord);
		}
	}
	SendCharacterSkillRecord(pUser, aChange);
}

