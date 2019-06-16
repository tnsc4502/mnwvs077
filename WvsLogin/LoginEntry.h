#pragma once
#include <string>

enum LoginResult
{
	res_PasswdCheck_Success,
	res_PasswdCheck_Invalid_Password = 0x04,
	res_PasswdCheck_Invalid_AccountName = 0x05,
	res_LoginStatus_Socket_AlreadyLoggedIn = 0x06,
	res_LoginStatus_Account_AlreadyLoggedIn = 0x07,
};

enum LoginState
{
	LS_Connection_Established = 0x00,
	LS_SelectGenderAnd2ndPassword = 0x01,
	LS_PasswdCheck_Authenticated = 0x02,
	LS_Stage_SelectedWorld = 0x03,
	LS_Stage_SelectedCharacter = 0x04,
	LS_Stage_MigratedIn = 0x05,
	LS_Stage_CheckDuplicatedID = 0x06,
};

struct LoginEntry
{
	int nAccountID,
		nWorldID,
		nChannelID;

	std::string strAccountName;
	char nGender;
	unsigned char nLoginState;
	unsigned int uLoginSocketSN;

	inline void Initialize()
	{
		nLoginState = LS_Connection_Established;
		nGender = -1;
		nAccountID = -1;
		nWorldID = -1;
		nChannelID = -1;
		strAccountName = "";
	}
};