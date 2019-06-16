#include "LoginDBAccessor.h"
#include "WvsUnified.h"
#include "..\WvsLogin\LoginEntry.h"

int LoginDBAccessor::CheckPassword(const std::string& sID, const std::string& sPasswd, int nTemporaryDue, int nWaitingDue, int & nAccountID, char & nGender)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * From Account Where AccountName = '" << sID << "'";
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	if (recordSet.rowCount() == 0)
		return LoginResult::res_PasswdCheck_Invalid_AccountName;
	if (recordSet["Password"].toString() != sPasswd)
		return LoginResult::res_PasswdCheck_Invalid_Password;
	nAccountID = recordSet["AccountID"];
	nGender = (int)recordSet["Gender"];
	return LoginResult::res_PasswdCheck_Success;
}

void LoginDBAccessor::UpdateGenderAnd2ndPassword(int nAccountID, int nGender, const std::string & s2ndPasswd)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "UPDATE Account Set SecondPassword = '" << s2ndPasswd << "', Gender = " << nGender << " WHERE AccountID = " << nAccountID;
	queryStatement.execute();
}
