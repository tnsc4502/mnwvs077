#pragma once
#include <string>

class LoginDBAccessor
{
public:
	static int CheckPassword(const std::string& sID, const std::string& sPasswd, int nTemporaryDue, int nWaitingDue, int& nAccountID, char& nGender);
	static void UpdateGenderAnd2ndPassword(int nAccountID, int nGender, const std::string& s2ndPasswd);
};

