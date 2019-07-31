#include "GameApp.h"
#include "WvsGame.h"
#include "User.h"
#include "..\WvsLib\String\StringUtility.h"

void GameApp::OnCommandPromptInput(std::string& sInput)
{
	if (sInput == "GetUserList")
	{
		std::lock_guard<std::recursive_mutex> lock(WvsBase::GetInstance<WvsGame>()->GetUserLock());
		auto& aUser = WvsBase::GetInstance<WvsGame>()->GetConnectedUser();
		std::string sOutput = "User List On Game Server: \n";

		for (auto& prUser : aUser)
			sOutput += "User [Name = " + prUser.second->GetName() + "] [User ID = " + std::to_string(prUser.second->GetUserID()) + "]\n";

		sOutput += StringUtility::Format("Total User Count: %d\n", (int)aUser.size());
		WvsLogger::LogRaw(("[Command][GetUserList]" + sOutput).c_str());
	}
}
