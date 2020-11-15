#pragma once
#include <vector>

struct GW_SkillRecord;
class User;

class QWUSkillRecord
{
	QWUSkillRecord();
	~QWUSkillRecord();
public:
	static void GetSkillRootFromJob(int nJob, std::vector<int> &aRet);
	static bool SkillUp(User *pUser, int nSkillID, int nAmount, bool bDecSP, bool bCheckRequiredSkill, bool bCheckMasterLevel, std::vector<GW_SkillRecord*> &aChange);
	static void SendCharacterSkillRecord(User *pUser, std::vector<GW_SkillRecord*> &aChange);
	static bool MasterLevelChange(User *pUser, int nSkillID, int nMasterLevel);
	static bool MakeSkillVisible(User *pUser, int nSkillID, int nSkillLevel, int nMasterLevel);
	static void ValidateMasterLevelForSKills(User *pUser);
	static bool IsValidSkill(User *pUser, void* pmNewSkillRecord, int nItemLevel, int nSkillRoot, bool bDec);
	static bool CanSkillChange(User *pUser, int nIncSkillID, int nDecSkillID, int nItemLevel);
};

