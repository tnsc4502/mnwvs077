#pragma once
#include <string>
#include <vector>

class InPacket;
class OutPacket;

struct GW_Friend
{
	int nCharacterID, nFriendID, nFlag, nChannelID;
	std::string sFriendName;

	GW_Friend();
	~GW_Friend();

	void Encode(OutPacket *oPacket);
	void Decode(InPacket *iPacket);

	void Load(int nCharacterID, int nFriendID);
	static std::vector<GW_Friend*> LoadAll(int nCharacterID);
	void Save();
	void Delete();
};

