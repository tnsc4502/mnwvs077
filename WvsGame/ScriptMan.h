#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "Script.h"

class Field;

class ScriptMan
{
public:

	static ScriptMan* GetInstance();
	Script* GetScript(const std::string& file, int nTemplateID, Field *pField);
};
