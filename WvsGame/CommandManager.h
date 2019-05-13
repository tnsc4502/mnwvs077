#pragma once
#include <string>
#include <vector>
#include <map>

class User;

class CommandManager
{
public:
	typedef const std::vector<std::string>& PARAM_TYPE;
	typedef int(*INVOKE_TYPE)(User *pUser, PARAM_TYPE aInput);

private:
	CommandManager();
	~CommandManager();

	std::map<std::string, INVOKE_TYPE> m_mCmdInvoke;

public:
	static CommandManager* GetInstance();
	void Process(User* pUser, const std::string& input);
};

