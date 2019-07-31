#pragma once
#include <string>

class GameApp
{
public:
	static void InitializeService(int argc, char **argv);
	static void OnCommandPromptInput(std::string& sInput);
};

