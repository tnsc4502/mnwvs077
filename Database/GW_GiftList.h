#pragma once
#include "..\WvsLib\Memory\ZMemory.h"
#include <vector>
#include <string>

class InPacket;
class OutPacket;

struct  GW_GiftList
{
	long long int liSN = -1, liItemSN = 0;
	int nItemID = 0, nCharacterID = 0, nState = 0;
	std::string sBuyCharacterName, sText;

#if defined(DBLIB) || defined(_WVSCENTER)
	void Load(void *pRecordSet);
	void Save();
#endif
	void Encode(OutPacket *oPacket);
	void Decode(InPacket *iPacket);

	static std::vector<ZUniquePtr<GW_GiftList>> Load(int nCharacterID);
};

