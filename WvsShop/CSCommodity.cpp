#include "CSCommodity.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Logger\WvsLogger.h"

CSCommodity::CSCommodity()
{
}


CSCommodity::~CSCommodity()
{
}

void CSCommodity::EncodeModifiedData(OutPacket * oPacket)
{
	oPacket->Encode2(nModifiedFlag);
	//WvsLogger::LogFormat("SN : %d, nItemID : %d, Modified Flag : %08X, & 0x08 ? %d OnSale = ?%d\n", nSN, nItemID, nModifiedFlag, (nModifiedFlag & 0x08), nOnSale);
	if (nModifiedFlag & ModifiedFlag::ItemID)
		oPacket->Encode4(nItemID);
	if (nModifiedFlag & ModifiedFlag::Count)
		oPacket->Encode2(nCount);
	if (nModifiedFlag & ModifiedFlag::Priority)
		oPacket->Encode1(nPriority);
	if (nModifiedFlag & ModifiedFlag::Price)
		oPacket->Encode4(nPrice);
	if (nModifiedFlag & ModifiedFlag::Period)
		oPacket->Encode2(nPeriod);
	if (nModifiedFlag & ModifiedFlag::MaplePoint)
		oPacket->Encode4(nMaplePoint);
	if (nModifiedFlag & ModifiedFlag::Meso)
		oPacket->Encode4(nMeso);
	if (nModifiedFlag & ModifiedFlag::ForPremiumUser)
		oPacket->Encode1(0);
	if (nModifiedFlag & ModifiedFlag::CommdityGender)
		oPacket->Encode1(nGender);
	if (nModifiedFlag & ModifiedFlag::OnSale)
		oPacket->Encode1(1);
	//0x400 --> nClass
	//oPacket->Encode2(1);
	if (nModifiedFlag & ModifiedFlag::ReqLev)
		oPacket->Encode1(0);
	//0x10000 --> bLimit
	if (nModifiedFlag & ModifiedFlag::PbCash)
		oPacket->Encode2(nPbCash);
	if (nModifiedFlag & ModifiedFlag::PbPoint)
		oPacket->Encode2(nPbPoint);
	if (nModifiedFlag & ModifiedFlag::PbGift)
		oPacket->Encode2(nPbGift);
	if (nModifiedFlag & ModifiedFlag::PackageSN)
	{
		oPacket->Encode1(0);
	}
}
