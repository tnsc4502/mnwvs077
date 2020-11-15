#pragma once

class User;

class QWUser
{
private:
	static bool TryProcessLevelUp(User* pUser, int nInc, int& refReachMaxLvl);

public:
	static bool IsValidStat(User *pUser, int nStr, int nDex, int nInt, int nLuk, int nRemainAP);
	static long long int CanStatChange(User *pUser, int nInc, int nDec);

	static long long int IncSTR(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncDEX(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncLUK(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncINT(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncMP(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncHP(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncPOP(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncSP(User* pUser, int nJobLevel, int nInc, bool bOnlyFull);
	static long long int IncMoney(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncAP(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncMHP(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncMMP(User* pUser, int nInc, bool bOnlyFull);
	static long long int IncEXP(User* pUser, int nInc, bool bOnlyFull);
	static int IncMaxHPVal(User *pUser, int nInc, int nDec);
	static int IncMaxMPVal(User *pUser, int nInc, int nDec);

	static int GetSTR(User* pUser);
	static int GetDEX(User* pUser);
	static int GetLUK(User* pUser);
	static int GetINT(User* pUser);
	static int GetMP(User* pUser);
	static int GetHP(User* pUser);
	static int GetMMP(User* pUser);
	static int GetMHP(User* pUser);
	static int GetPOP(User* pUser);
	static int GetSP(User* pUser, int nJobLevel);
	static long long int GetMoney(User* pUser);
	static int GetAP(User* pUser);
	static long long int GetEXP(User* pUser);
	static short GetJob(User* pUser);
	static unsigned char GetLevel(User* pUser);

	static long long int SetFace(User *pUser, int nFace);
	static long long int SetHair(User *pUser, int nHair);
	static long long int SetJob(User *pUser, int nJob);
	static long long int SetLevel(User *pUser, int nLevel);
	static long long int SetSkin(User *pUser, int nSkin);
	static long long int SetMoney(User *pUser, int nAmount);
};

