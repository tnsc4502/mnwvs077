#include "QWUser.h"
#include "User.h"
#include "SecondaryStat.h"
#include "QWUSkillRecord.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterLevel.h"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_Avatar.hpp"

bool QWUser::TryProcessLevelUp(User *pUser, int nInc, int & refReachMaxLvl)
{
	//if (UtilUser::m_nEXP[1] == 0)
	//	UtilUser::LoadEXP();
	auto& ref = pUser->GetCharacterData()->mStat->nExp;
	ref += nInc;
	if (pUser->GetCharacterData()->mLevel->nLevel < 200 &&
		ref >= UserUtil::m_nEXP[pUser->GetCharacterData()->mLevel->nLevel])
	{
		ref -= UserUtil::m_nEXP[pUser->GetCharacterData()->mLevel->nLevel];
		++pUser->GetCharacterData()->mLevel->nLevel;
		
		//Could only increase one level no matter how many EXPs are earned.
		if (ref >= UserUtil::m_nEXP[pUser->GetCharacterData()->mLevel->nLevel])
			ref = UserUtil::m_nEXP[pUser->GetCharacterData()->mLevel->nLevel] - 1;

		if (pUser->GetCharacterData()->mLevel->nLevel >= 200)
			refReachMaxLvl = 1;
		return true;
	}
	return false;
}

bool QWUser::IsValidStat(User * pUser, int nStr, int nDex, int nInt, int nLuk, int nRemainAP)
{
	int nJob = GetJob(pUser);
	if (nStr < 4 ||
		nDex < 4 ||
		nInt < 4 ||
		nLuk < 4 ||
		((nJob / 100 == 1) && nStr < 35) ||
		((nJob / 100 == 2) && nInt < 20) ||
		((nJob / 100 == 3 || nJob / 100 == 4) && nLuk < 25))
		return false;
	else
	{
		int nRequiredExtraAP = (nJob % 10 == 1) ? 5 : (nJob % 10 != 2 ? 0 : 10);

		//WTF
		/*return
			nRemainAP + nLuk + nInt + nDex + nStr <=
			nRequiredExtraAP + 4 * (pUser->GetCharacterData()->mLevel->nLevel + 4) + pUser->GetCharacterData()->mLevel->nLevel + 4;*/
		return true;
	}
}

long long int QWUser::CanStatChange(User * pUser, int nInc, int nDec)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nJob = GetJob(pUser);

	if (nInc == nDec || ((nJob / 100 < 0) || (nJob / 100 > 5)))
		return 0;

	int nStr = GetSTR(pUser),
		nDex = GetDEX(pUser),
		nInt = GetINT(pUser),
		nLuk = GetLUK(pUser);

	if (nDec == BasicStat::BS_MaxHP)
	{
		int nHPBound = 0;
		if(!nJob == 0)
			nHPBound = 12 * GetLevel(pUser) + 38;
		else if (UserUtil::IsAdventurerWarrior(nJob)) 
		{
			if (nJob >= 110 && nJob <= 112)
				nHPBound = 24 * GetLevel(pUser) + 418;
			else
				nHPBound = 24 * GetLevel(pUser) + 118;
		}
		if (UserUtil::IsAdventurerMagic(nJob))
			nHPBound = 10 * GetLevel(pUser) + 54;
		if (UserUtil::IsAdventurerArcher(nJob) || UserUtil::IsAdventurerThief(nJob))
			nHPBound = 20 * GetLevel(pUser) + ((nJob % 10 == 0) ? 54 : 358);

		if (nHPBound > IncMaxHPVal(pUser, 0, nDec) + pUser->GetCharacterData()->mStat->nMaxHP)
			return 0;
	}
	else if (nDec == BasicStat::BS_MaxMP)
	{
		int nMPBound = 0;
		if (!nJob == 0)
			nMPBound = 10 * GetLevel(pUser) - 5;
		else if (UserUtil::IsAdventurerWarrior(nJob))
		{
			if (nJob >= 110 && nJob <= 112)
				nMPBound = 4 * GetLevel(pUser) + 55;
			else
				nMPBound = 4 * GetLevel(pUser) + 115;
		}
		if (UserUtil::IsAdventurerMagic(nJob))
			nMPBound = 22 * GetLevel(pUser) + ((nJob % 10 == 0) ? -1 : 449);
		if (UserUtil::IsAdventurerArcher(nJob) || UserUtil::IsAdventurerThief(nJob))
			nMPBound = 14 * GetLevel(pUser) + ((nJob % 10 == 0) ? -15 : 135);

		if (nMPBound > IncMaxMPVal(pUser, 0, nDec) + pUser->GetCharacterData()->mStat->nMaxMP)
			return 0;
	}
	else
	{
		if (nDec == BasicStat::BS_STR)
			--nStr;
		else if (nDec == BasicStat::BS_DEX)
			--nDex;
		else if (nDec == BasicStat::BS_INT)
			--nInt;
		else if (nDec == BasicStat::BS_LUK)
			--nLuk;
		else
			return 0;

		if (!IsValidStat(pUser, nStr, nDex, nInt, nLuk, GetAP(pUser) + 1))
			return 0;
	}

	int nStatToCheck = 0, nIncStat = 0;
	switch (nInc)
	{
		case BasicStat::BS_STR:
			++nStr;
			break;
		case BasicStat::BS_DEX:
			++nDex;
			break;
		case BasicStat::BS_INT:
			++nInt;
			break;
		case BasicStat::BS_LUK:
			++nLuk;
			break;
		case BasicStat::BS_MaxHP:
			nIncStat = IncMaxHPVal(pUser, nInc, 0);
			nStatToCheck = pUser->GetCharacterData()->mStat->nMaxHP;
			break;
		case BasicStat::BS_MaxMP:
			nIncStat = IncMaxMPVal(pUser, nInc, 0);
			nStatToCheck = pUser->GetCharacterData()->mStat->nMaxMP;
			break;
	}
	if (nIncStat && (nIncStat + nStatToCheck) > 300000)
		return 0;

	long long int liFlag = 0;
	if (IsValidStat(pUser, nStr, nDex, nInt, nLuk, GetAP(pUser)))
	{
		liFlag |= QWUser::IncSTR(pUser, (nInc == BasicStat::BS_STR) * 1 + (nDec == BasicStat::BS_STR) * -1, false);
		liFlag |= QWUser::IncDEX(pUser, (nInc == BasicStat::BS_DEX) * 1 + (nDec == BasicStat::BS_DEX) * -1, false);
		liFlag |= QWUser::IncINT(pUser, (nInc == BasicStat::BS_INT) * 1 + (nDec == BasicStat::BS_INT) * -1, false);
		liFlag |= QWUser::IncLUK(pUser, (nInc == BasicStat::BS_LUK) * 1 + (nDec == BasicStat::BS_LUK) * -1, false);
		liFlag |= QWUser::IncMHP(pUser, IncMaxHPVal(pUser, nInc, nDec), false);
		liFlag |= QWUser::IncMMP(pUser, IncMaxMPVal(pUser, nInc, nDec), false);
	}

	return liFlag;
}

long long int QWUser::IncSTR(User *pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto& refValue = pUser->GetCharacterData()->mStat->nStr;
	if (bOnlyFull && refValue + nInc < 0 || refValue + nInc > 999)
		return 0;
	refValue += nInc;
	pUser->ValidateStat();

	return BasicStat::BS_STR;
}

long long int QWUser::IncDEX(User *pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto& refValue = pUser->GetCharacterData()->mStat->nDex;
	if (bOnlyFull && refValue + nInc < 0 || refValue + nInc > 999)
		return 0;
	refValue += nInc;
	pUser->ValidateStat();

	return BasicStat::BS_DEX;
}

long long int QWUser::IncLUK(User *pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto& refValue = pUser->GetCharacterData()->mStat->nLuk;
	if (bOnlyFull && refValue + nInc < 0 || refValue + nInc > 999)
		return 0;
	refValue += nInc;
	pUser->ValidateStat();

	return BasicStat::BS_LUK;
}

long long int QWUser::IncINT(User *pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto& refValue = pUser->GetCharacterData()->mStat->nInt;
	if (bOnlyFull && refValue + nInc < 0 || refValue + nInc > 999)
		return 0;
	refValue += nInc;
	pUser->ValidateStat();

	return BasicStat::BS_INT;
}

long long int QWUser::IncMP(User *pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	if (nInc < 0 && pUser->GetSecondaryStat()->nInfinity_ > 0)
		return 0;
	int nMP = pUser->GetCharacterData()->mStat->nMP;
	int nMaxMP = pUser->GetBasicStat()->nMMP;

	nMP += nInc;
	if (nMP < 0)
		nMP = 0;
	if (nMP >= nMaxMP)
		nMP = nMaxMP;
	pUser->GetCharacterData()->mStat->nMP = nMP;
	pUser->ValidateStat();

	return BasicStat::BS_MP;
}

long long int QWUser::IncHP(User *pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nHP = pUser->GetCharacterData()->mStat->nHP;
	int nMaxHP = pUser->GetBasicStat()->nMHP;

	nHP += nInc;
	if (nHP < 0)
		nHP = 0;
	if (nHP >= nMaxHP)
		nHP = nMaxHP;
	pUser->GetCharacterData()->mStat->nHP = nHP;
	pUser->ValidateStat();

	return BasicStat::BS_HP;
}

long long int QWUser::IncPOP(User *pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nPOP = pUser->GetCharacterData()->mStat->nPOP;
	nPOP += nInc;
	if (bOnlyFull && (nPOP > 30000 || nPOP < -30000))
		return 0;
	if (nPOP > 30000)
		nPOP = 30000;
	if (nPOP < -30000)
		nPOP = -30000;
	pUser->GetCharacterData()->mStat->nPOP = nPOP;
	pUser->ValidateStat();

	return BasicStat::BS_POP;
}

long long int QWUser::IncSP(User *pUser, int nJobLevel, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	if (nJobLevel < 0 || nJobLevel >= GW_CharacterStat::EXTEND_SP_SIZE)
		return 0;
	auto& ref = pUser->GetCharacterData()->mStat->aSP[0];
	if (bOnlyFull && (ref + nInc < 0 || ref + nInc > 255))
		return 0;
	ref += nInc;
	if (ref < 0)
		ref = 0;
	if (ref >= 255)
		ref = 255;
	pUser->ValidateStat();

	return BasicStat::BS_SP;
}

long long int QWUser::IncMoney(User *pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	pUser->GetCharacterData()->mMoney->nMoney += nInc;
	return BasicStat::BS_Meso;
}

long long int QWUser::IncAP(User *pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nAP = pUser->GetCharacterData()->mStat->nAP;
	nAP += nInc;
	if (bOnlyFull && (nAP > 255 || nAP < 0))
		return 0;
	if (nAP > 255)
		nAP = 255;
	if (nAP < 0)
		nAP = 0;
	pUser->GetCharacterData()->mStat->nAP = nAP;
	pUser->ValidateStat();

	return BasicStat::BS_AP;
}

long long int QWUser::IncMHP(User *pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nMaxHP = pUser->GetCharacterData()->mStat->nMaxHP;
	nMaxHP += nInc;
	if (bOnlyFull && (nMaxHP > 30000 || nMaxHP < 0))
		return 0;
	if (nMaxHP > 30000)
		nMaxHP = 30000;
	if (nMaxHP < 0)
		nMaxHP = 0;
	pUser->GetCharacterData()->mStat->nMaxHP = nMaxHP;
	pUser->ValidateStat();

	return BasicStat::BS_MaxHP;
}

long long int QWUser::IncMMP(User *pUser, int nInc, bool bOnlyFull)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nMaxMP = pUser->GetCharacterData()->mStat->nMaxMP;
	nMaxMP += nInc;
	if (bOnlyFull && (nMaxMP > 30000 || nMaxMP < 0))
		return 0;
	if (nMaxMP > 30000)
		nMaxMP = 30000;
	if (nMaxMP < 0)
		nMaxMP = 0;
	pUser->GetCharacterData()->mStat->nMaxMP = nMaxMP;
	pUser->ValidateStat();

	return BasicStat::BS_MaxMP;
}

long long int QWUser::IncEXP(User *pUser, int nInc, bool bOnlyFull)
{
	long long int nRet= BasicStat::BS_EXP;
	int nReachMaxLvl = 0;
	if (TryProcessLevelUp(pUser, nInc, nReachMaxLvl))
	{
		nRet |= BasicStat::BS_Level;
		nRet |= IncAP(
			pUser,
			/*WvsGameConstants::IsCygnusJob(pUser->GetCharacterData()->mStat->nJob) ? 6 : */ 5,
			false);

		if(QWUser::GetJob(pUser) != 0)
			nRet |= IncSP(
				pUser,
				UserUtil::GetJobLevel(pUser->GetCharacterData()->mStat->nJob),
				3,
				false);
		pUser->OnLevelUp();
	}
	pUser->ValidateStat();
	return nRet;
}

int QWUser::IncMaxHPVal(User * pUser, int nInc, int nDec)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nJobType = pUser->GetCharacterData()->mStat->nJob / 100;

	nInc = (nInc == BasicStat::BS_MaxHP ? 1 : 0);
	nDec = (nDec == BasicStat::BS_MaxHP ? 1 : 0);
	switch (nJobType)
	{
		case 0:
			return 8 * nInc + 12 * -nDec;
		case 1:
			return 20 * nInc + 54 * -nDec;
		case 2:
			return 6 * nInc + 10 * -nDec;
		case 3:
		case 4:
			return 16 * nInc + 20 * -nDec;
	}

	return 0;
}

int QWUser::IncMaxMPVal(User * pUser, int nInc, int nDec)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nJobType = pUser->GetCharacterData()->mStat->nJob / 100;

	nInc = (nInc == BasicStat::BS_MaxHP ? 1 : 0);
	nDec = (nDec == BasicStat::BS_MaxHP ? 1 : 0);
	switch (nJobType)
	{
		case 0:
			return 6 * nInc + 8 * -nDec;
		case 1:
			return 2 * nInc + 4 * -nDec;
		case 2:
			return 18 * nInc + (-30 - 3 * GetINT(pUser) / 45) * nDec; //nDec with no negative sign here plz.
		case 3:
		case 4:
			return 10 * nInc + 12 * -nDec;
	}
	return 0;
}

int QWUser::GetSTR(User *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetBasicStat()->nSTR;
}

int QWUser::GetDEX(User *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetBasicStat()->nDEX;
}

int QWUser::GetLUK(User *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetBasicStat()->nLUK;
}

int QWUser::GetINT(User *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetBasicStat()->nINT;
}

int QWUser::GetMP(User *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mStat->nMP;
}

int QWUser::GetHP(User *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mStat->nHP;
}

int QWUser::GetMMP(User *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetBasicStat()->nMMP;
}

int QWUser::GetMHP(User *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetBasicStat()->nMHP;
}

int QWUser::GetPOP(User *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mStat->nPOP;
}

int QWUser::GetSP(User *pUser, int nJobLevel)
{
	return pUser->GetCharacterData()->mStat->aSP[nJobLevel];
}

long long int QWUser::GetMoney(User *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mMoney->nMoney;
}

int QWUser::GetAP(User *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mStat->nAP;
}

long long int QWUser::GetEXP(User *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mStat->nExp;
}

short QWUser::GetJob(User *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mStat->nJob;
}

unsigned char QWUser::GetLevel(User *pUser)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mLevel->nLevel;
}

long long int QWUser::SetFace(User *pUser, int nFace)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	pUser->GetCharacterData()->mStat->nFace = nFace;
	pUser->GetCharacterData()->mAvatarData->nFace = nFace;
	pUser->ValidateStat();

	return BasicStat::BS_Face;
}

long long int QWUser::SetHair(User *pUser, int nHair)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	pUser->GetCharacterData()->mStat->nHair = nHair;
	pUser->GetCharacterData()->mAvatarData->nHair = nHair;
	pUser->ValidateStat();

	return BasicStat::BS_Hair;
}

long long int QWUser::SetJob(User *pUser, int nJob)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	pUser->GetCharacterData()->mStat->nJob = nJob;
	QWUSkillRecord::ValidateMasterLevelForSKills(pUser);
	pUser->ValidateStat();

	return BasicStat::BS_Job;
}

long long int QWUser::SetLevel(User *pUser, int nLevel)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	pUser->GetCharacterData()->mLevel->nLevel = nLevel;
	pUser->ValidateStat();

	return BasicStat::BS_Level;
}

long long int QWUser::SetSkin(User *pUser, int nSkin)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	pUser->GetCharacterData()->mStat->nSkin = nSkin;
	pUser->GetCharacterData()->mAvatarData->nSkin = nSkin;
	pUser->ValidateStat();

	return BasicStat::BS_Skin;
}

long long int QWUser::SetMoney(User *pUser, int nAmount)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	pUser->GetCharacterData()->mMoney->nMoney = nAmount;
	return BasicStat::BS_Meso;
}
