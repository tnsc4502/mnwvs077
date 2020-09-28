#include "PacketTypes.hpp"

static std::map<int, std::string>* ms_mPacketNameMap = nullptr;

const std::string & PacketType::GetTypeName(int nType)
{
	static const std::string sEmpty = "";
	auto findIter = (*ms_mPacketNameMap).find(nType);
	return findIter == (*ms_mPacketNameMap).end() ? sEmpty : findIter->second;
}

PacketType::PacketType(const char *sClassName, int nValue, const char *sName)
{
	if (!ms_mPacketNameMap)
		ms_mPacketNameMap = new std::map<int, std::string>();

	std::string sTypeName = sClassName;
	sTypeName = sTypeName.substr(7);
	sTypeName += "::";
	sTypeName += sName;

	m_nValue = nValue;
	m_sName = sName;
	(*ms_mPacketNameMap).insert({ nValue, sTypeName });
}