#pragma once
#include <string>

class ShopApp
{
public:
	static void InitializeService(int argc, char **argv);
	static void OnCommandPromptInput(std::string& sInput);
};

