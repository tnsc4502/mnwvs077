#include "GameApp.h"
#include "WvsGame.h"
#include "User.h"
#include "..\WvsLib\String\StringUtility.h"

const std::string& Get(std::vector<std::string>& aInput, int nIdx)
{
	if (nIdx >= aInput.size())
		throw std::exception(StringUtility::Format("Invalid token fetch (fetch index = %d).", nIdx).c_str());

	return (aInput[nIdx]);
}

int GetInt(std::vector<std::string>& aInput, int nIdx)
{
	return atoi(Get(aInput, nIdx).c_str());
}

void GameApp::OnCommandPromptInput(std::string& sInput)
{
	std::string sOutput;
	std::vector<std::string> asTokens;
	StringUtility::Split(sInput, asTokens, " ");
	std::string& sCommand = asTokens[0], sUsage = "";
	
	try 
	{
		if (sCommand == "GetUserList")
		{
			std::lock_guard<std::recursive_mutex> lock(WvsBase::GetInstance<WvsGame>()->GetUserLock());
			auto& aUser = WvsBase::GetInstance<WvsGame>()->GetConnectedUser();
			sOutput = "User List On Game Server: \n";

			for (auto& prUser : aUser)
				sOutput += "User [Name = " + prUser.second->GetName() + "] [User ID = " + std::to_string(prUser.second->GetUserID()) + "]\n";

			sOutput += StringUtility::Format("Total User Count: %d\n", (int)aUser.size());
		}
		else if (sCommand == "GetUserInfo")
		{
			sUsage = "Usage: GetUserInfo <std::string: User Name>";
			auto sUserName = Get(asTokens, 1);
		}
		else
			sOutput = "Unrecognized command.\n";
	}
	catch (std::exception& ex) 
	{
		sOutput = std::string("Error occurred. ") + ex.what() + "\n";
		sOutput += sUsage + "\n";
	}
	if (sOutput != "")
		WvsLogger::LogRaw(("[Command][" + sCommand + "]" + sOutput).c_str());
}
