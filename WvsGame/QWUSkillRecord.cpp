#include "QWUSkillRecord.h"
#include "QWUser.h"
#include "User.h"
#include "SkillInfo.h"
#include "SkillEntry.h"

#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsGame\UserPacketTypes.hpp"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_SkillRecord.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

#include <algorithm>

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

bool QWUSkillRecord::SkillUp(User * pUser, int nSkillID, int nAmount, bool bDecSP, bool bCheckRequiredSkill, bool bCheckMasterLevel, std::vector<GW_SkillRecord*>& aChange)
{
	int nJob = pUser->GetCharacterData()->mStat->nJob;
	int nSkillJob = SkillInfo::GetSkillRootFromSkill(nSkillID);

	auto pSkill = SkillInfo::GetInstance()->GetSkillByID(nSkillID);
	if (pSkill &&
		(nJob >= nSkillJob) &&
		((nJob < 10000 && (nJob / 100 == nSkillJob / 100)) ||
		(nJob / 1000 == nSkillJob / 1000)))
	{
		int nSkillRootLevel = UtilUser::GetJobLevel(nSkillJob);
		if (/*nSkillRootLevel >= 0 &&
			nSkillRootLevel < GW_CharacterStat::EXTEND_SP_SIZE &&*/
			(!bDecSP || pUser->GetCharacterData()->mStat->aSP[0] >= nAmount))
		{
			auto pSkillRecord = pUser->GetCharacterData()->GetSkill(nSkillID);
			if (pSkillRecord == nullptr)
			{
				auto& aReqSkill = pSkill->GetRequiredSkill();

				//Check Required Skill Level
				if (bCheckRequiredSkill)
				{
					for (auto& prReq : aReqSkill)
						if (SkillInfo::GetInstance()->GetSkillLevel(pUser->GetCharacterData(), prReq.first, nullptr) < prReq.second)
							return false;
				}

				pSkillRecord = SkillInfo::GetInstance()->GetSkillRecord(
					nSkillID,
					0,
					0
				);
				pUser->GetCharacterData()->ObtainSkillRecord(pSkillRecord);
			}
			if (pSkillRecord != nullptr 
				//Check Master Level.
				&& (!bCheckMasterLevel || (!SkillInfo::IsSkillNeedMasterLevel(pSkillRecord->nSkillID) || (pSkillRecord->nSLV + nAmount <= pSkillRecord->nMasterLevel)))
				)
			{
				nAmount = std::min(
					nAmount,
					pSkill->GetMaxLevel() - pSkillRecord->nSLV);

				pSkillRecord->nSLV += nAmount;

				if(!bCheckMasterLevel)
					pSkillRecord->nMasterLevel = pSkill->GetMaxLevel();
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
	oPacket.Encode2(UserSendPacketType::UserLocal_OnChangeSkillRecordResult);
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
	auto &cd = pUser->GetCharacterData();
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
	auto &cd = pUser->GetCharacterData();
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

bool QWUSkillRecord::IsValidSkill(User *pUser, void* pmNewSkillRecord, int nItemLevel, int nSkillRoot, bool bDec)
{
	auto & mNewSkillRecord = *(std::map<int, ZSharedPtr<GW_SkillRecord>>*)pmNewSkillRecord;
	int nSP[4] = { 0 };
	for (auto& prRecord : mNewSkillRecord)
	{
		auto& pSkillRecord = prRecord.second;
		nSP[UtilUser::GetJobLevel(pSkillRecord->nSkillID / 10000) - 1] += pSkillRecord->nSLV;
	}

	int nJob = QWUser::GetJob(pUser);
	int nJobLevel = UtilUser::GetJobLevel(nJob);
	int nSPChekc =
		nSP[0]
		+ nSP[1]
		+ nSP[2]
		+ nSP[3]
		+ QWUser::GetSP(pUser, 0)
		+ (nJob / 100 != 2 ? 0 : 6) //Extra two levels
		+ 3 * ((nJob / 100 != 2 ? 10 : 8) - QWUser::GetLevel(pUser))
		- (nJobLevel == 4 ? 2 : 0)//Extra points given by job adv?
		- nJobLevel //Extra points given by job adv?
		+ 61; //Maybe 10~30 ?

	for (auto& prRecord : mNewSkillRecord)
	{
		auto& pSkillRecord = prRecord.second;
		auto pSkill = SkillInfo::GetInstance()->GetSkillByID(pSkillRecord->nSkillID);
		if (!pSkill)
			continue;

		auto& aReqSkill = pSkill->GetRequiredSkill();
		for (auto& prReqSkill : aReqSkill)
		{
			auto findReqSkill = mNewSkillRecord.find(prReqSkill.first);
			auto pReqSkill = (findReqSkill == mNewSkillRecord.end() ? nullptr : (GW_SkillRecord*)findReqSkill->second);

			if (!pReqSkill || pReqSkill->nSLV < prReqSkill.second)
				return false;
		}
	}
	
	if (nItemLevel != UtilUser::GetJobLevel(nSkillRoot))
	{
		if (nItemLevel == 2)
			return nSP[0] - nSPChekc >= 0;
		else if (nItemLevel == 3)
			return ((nSP[0] + nSP[1]) - (nSPChekc + (bDec ? 120 : 121))) >= 0; //2nd ~ 3rd = (70 - 30) * 3 points
		else
		{
			if (nItemLevel != 4)
				return true;
			return ((nSP[0] + nSP[1] + nSP[2]) - (nSPChekc + (bDec ? 120 : 121) + (bDec ? 150 : 151))) >= 0; //2nd ~ 4th = (120 - 30) * 3 points
		}
	}
	return false;
}

bool QWUSkillRecord::CanSkillChange(User * pUser, int nIncSkillID, int nDecSkillID, int nItemLevel)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nJob = QWUser::GetJob(pUser);

	if ((nJob / 100) < 1 || (nJob / 100) > 5 || UtilUser::GetJobLevel(nJob) < nItemLevel)
		return false;

	int nIncSkillRoot = nIncSkillID / 10000,
		nDecSkillRoot = nDecSkillID / 10000;

	if (nItemLevel == UtilUser::GetJobLevel(nIncSkillRoot))
	{
		std::vector<int> anSkillRoot;
		GetSkillRootFromJob(nJob, anSkillRoot);
		bool bIncSkillRootContained = false, bDecSkillRootContained = false;

		for (int i = 0; i <= (int)anSkillRoot.size(); ++i)
		{
			if (i == (int)anSkillRoot.size())
				return false;

			if (anSkillRoot[i] == nIncSkillRoot)
				bIncSkillRootContained = true;

			if (anSkillRoot[i] == nDecSkillID)
				bDecSkillRootContained = true;
		}
		if (bIncSkillRootContained && bDecSkillRootContained && nIncSkillRoot >= nDecSkillRoot)
		{
			//Copy the whole skill records.
			std::map <int ,ZUniquePtr<GW_SkillRecord>> mNewSkillRecord;
			for (auto& prSkillRecord : pUser->GetCharacterData()->mSkillRecord) 
			{
				auto pNewRecord = AllocObj(GW_SkillRecord);
				*pNewRecord = *(prSkillRecord.second);
				mNewSkillRecord[prSkillRecord.first].reset(pNewRecord);
			}

			//Find Inc/Dec skill entries & records.
			auto pIncSkill = SkillInfo::GetInstance()->GetSkillByID(nIncSkillID),
				pDecSkill = SkillInfo::GetInstance()->GetSkillByID(nDecSkillID);

			auto findIncSkillRecord = mNewSkillRecord.find(nIncSkillID),
				findDecSkillRecord = mNewSkillRecord.find(nDecSkillID),
				endRecord = mNewSkillRecord.end();

			GW_SkillRecord* pIncSkillRecord = findIncSkillRecord == endRecord ? nullptr : (GW_SkillRecord*)findIncSkillRecord->second;
			GW_SkillRecord* pDecSkillRecord = findDecSkillRecord == endRecord ? nullptr : (GW_SkillRecord*)findDecSkillRecord->second;

			//Check possibility of inc/dec
			if (pIncSkill &&
				pDecSkill &&
				pDecSkillRecord &&
				pDecSkillRecord->nSLV >= 1 &&
				(!pIncSkillRecord || pIncSkillRecord->nSLV + 1 <= pIncSkill->GetMaxLevel()))
			{
				--pIncSkillRecord->nSLV;
				if (!IsValidSkill(pUser, &mNewSkillRecord, nItemLevel, nDecSkillRoot, true))
					return false;
			}
		}
	}

	return true;
}

