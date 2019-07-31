#pragma once
#include <string>

class CenterApp
{
public:
	static void InitializeService(int argc, char **argv);
	static void OnCommandPromptInput(std::string& sInput);
};

