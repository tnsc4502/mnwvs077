#pragma once
#include <string>

class LoginDBAccessor
{
public:
	static int CheckPassword(const std::string& sID, const std::string& sPasswd, int nTemporaryDue, int nWaitingDue, int& nAccountID, char& nGender);

};

