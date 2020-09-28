#pragma once

#include <map>
#include <string>

#define FlagMin(FlagName) FlagName::Min()
#define FlagMax(FlagName) FlagName::Max()

struct PacketType
{
	PacketType(const char *sClassName, int nValue, const char *sName);
	static const std::string& GetTypeName(int nType);

	int m_nValue;
	const char *m_sName;
};

#define DEFINE_PACKET_TYPE(name) \
struct name {\
private:\
public:\
	static name* name##Instance() { static name instance; return &instance; }\
	static int Min() { return ((PacketType*)name##Instance())->m_nValue; } \
	static int Max() { return *(int*)((char*)name##Instance() + sizeof(name) - sizeof(PacketType)); }

#define END_PACKET_TYPE(name) }; namespace __StaticInitializer##name { const static name __initializer; }

#define REGISTER_TYPE(name_, val) private: const PacketType __##name_ = { typeid(*this).name(), val, #name_ }; public: enum { name_ = val }