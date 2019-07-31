#pragma once
#include <string>

class LoginApp
{
public:
	static void InitializeService(int argc, char **argv);
	static void OnCommandPromptInput(std::string& sInput);
};

