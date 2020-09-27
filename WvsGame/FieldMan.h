#pragma once
#include <string>
#include <map>
#include <mutex>

class FieldSet;
class Field;

class FieldMan
{
private:
	std::mutex m_mtxFieldMan;

	std::map<int, Field*> m_mField;
	std::map<int, int> m_mAreaCode;
	std::map<std::string, FieldSet*> m_mFieldSet;

	FieldMan();

public:

	static FieldMan *GetInstance();
	void RegisterField(int nFieldID);
	void FieldFactory(int nFieldID);
	void LoadAreaCode();
	bool IsConnected(int nFrom, int nTo);
	void LoadFieldSet();
	void RegisterAllField();
	Field* GetField(int nFieldID);
	FieldSet* GetFieldSet(const std::string& sFieldSetName);
	void RestoreFoothold(Field* pField, void *pPropFoothold, void *pLadderOrRope, void *pInfo);
	~FieldMan();
};

