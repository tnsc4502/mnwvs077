#pragma once
#include "..\WvsLib\Memory\ZMemory.h"
#include <vector>

class InPacket;
class OutPacket;

struct GW_WishList
{
	const static int MAX_WISHLIST_COUNT = 10;
	int nCharacterID = 0, nValidWishList = 0;
	std::vector<int> aWishList;

#if defined(DBLIB) || defined(_WVSCENTER)
	void Save();
#endif
	void Encode(OutPacket *oPacket);
	void Decode(InPacket *iPacket);

	static ZUniquePtr<GW_WishList> Load(int nCharacterID);
};

