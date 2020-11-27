#include "GW_WishList.h"
#include "WvsUnified.h"

#include "..\WvsLib\String\StringUtility.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

void GW_WishList::Save()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);

	std::string sData = "";
	for (int i = 0; i < MAX_WISHLIST_COUNT; ++i)
		sData += std::to_string(aWishList[i]) + (i == MAX_WISHLIST_COUNT - 1 ? "" : ",");

	queryStatement << "INSERT INTO WishList VALUES("
	<< nCharacterID << ", "
	<< "\'" << sData << "\') ON DUPLICATE KEY UPDATE "
	<< "Data = \'" << sData << "\'";

	queryStatement.execute();
}

void GW_WishList::Encode(OutPacket * oPacket)
{
	for (int i = 0; i < MAX_WISHLIST_COUNT; ++i)
		oPacket->Encode4(aWishList[i]);
}

void GW_WishList::Decode(InPacket * iPacket)
{
	int nWishList = 0;
	aWishList.resize(MAX_WISHLIST_COUNT, 0);
	for (int i = 0; i < MAX_WISHLIST_COUNT; ++i) 
	{
		nWishList = iPacket->Decode4();
		aWishList[i] = nWishList;

		if (nWishList)
			++nValidWishList;
	}
}

ZUniquePtr<GW_WishList> GW_WishList::Load(int nCharacterID)
{
	ZUniquePtr<GW_WishList> pWishList = AllocObj(GW_WishList);

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT Data FROM WishList Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	
	std::string sData = recordSet.rowCount() ? recordSet["Data"].toString() : "";

	if(sData != "")
		StringUtility::Split(sData, pWishList->aWishList, ",");
	else
		pWishList->aWishList.resize(MAX_WISHLIST_COUNT, 0);

	return pWishList;
}
