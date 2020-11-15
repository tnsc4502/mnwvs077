#pragma once

struct GA_Character;
struct GW_ItemSlotBase;
class SecondaryStat;

class BasicStat
{
public:
	enum BasicStatFlag : long long int
	{
		BS_Skin = 0x01,
		BS_Face = 0x02,
		BS_Hair = 0x04,
		BS_Pet1 = 0x08,
		BS_Pet2 = 0x10,
		BS_Pet3 = 0x20,
		BS_Pet = BS_Pet1 | BS_Pet2 | BS_Pet3, //= 0x38
		BS_Level = 0x40,
		BS_Job = 0x80,
		BS_STR = 0x100,
		BS_DEX = 0x200,
		BS_INT = 0x400,
		BS_LUK = 0x800,
		BS_HP = 0x1000,
		BS_MaxHP = 0x2000,
		BS_MP = 0x4000,
		BS_MaxMP = 0x8000,
		BS_AP = 0x10000,
		BS_SP = 0x20000,
		BS_EXP = 0x40000,
		BS_POP = 0x80000,
		BS_Meso = 0x100000,
		BS_GachaponEXP = 0x200000
	};

	int nGender = 0,
		nLevel = 0,
		nJob = 0,
		nSTR = 0,
		nINT = 0,
		nDEX = 0,
		nLUK = 0,
		nPOP = 0,
		nMHP = 0,
		nMMP = 0,
		nBasicStatInc = 0,
		nCharismaEXP = 0,
		nInsightEXP = 0,
		nWillEXP = 0,
		nSenseEXP = 0,
		nCharmEXP = 0,
		nIncMaxHPr = 0,
		nIncMaxMPr = 0,
		nPsdEVAX = 0,
		nIncDEXr = 0,
		nIncSTRr = 0,
		nIncINTr = 0,
		nIncLUKr = 0,
		nSpiritLinkIn = 0,
		nIncMaxHPr_Forced = 0,
		nIncRMAF = 0,
		nIncRMAI = 0,
		nIncRMAL = 0,
		nElemDefault = 0
		;

public:
	BasicStat();
	~BasicStat();

	void SetPermanentSkillStat(GA_Character* pChar);
	void SetFrom(GA_Character* pChar, int nMaxHPIncRate, int nMaxMPIncRate, int nBasicStatInc);
};

