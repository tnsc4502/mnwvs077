#pragma once
class OutPacket;
class InPacket;

struct GW_SkillRecord
{
	int nSLV = 0, nSkillID = 0, nMasterLevel = 0, nCharacterID = 0;

	long long int tExpired;

	void Encode(OutPacket* oPacket);
	void Decode(InPacket* iPacket);

#ifdef DBLIB
	void Load(void *pRecordSet);
	void Save();
#endif
};

