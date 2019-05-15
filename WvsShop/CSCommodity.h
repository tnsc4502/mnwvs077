#pragma once
class OutPacket;

struct CSCommodity
{
	CSCommodity();
	~CSCommodity();

	void EncodeModifiedData(OutPacket *oPacket);

	enum ModifiedFlag
	{
		ItemID = 0x01,
		Count = 0x02,
		Price = 0x04,
		Priority = 0x08,
		Period = 0x10,
		MaplePoint = 0x20,
		Meso = 0x40,
		ForPremiumUser = 0x80,
		CommdityGender = 0x100,
		OnSale = 0x200,
		ReqLev = 0x800,
		PbCash = 0x1000,
		PbPoint = 0x2000,
		PbGift = 0x4000,
		PackageSN = 0x8000,
		Limit = 0x10000,
	};

	int nSN = 0,
		nItemID = 0,
		nCount = 0,
		nPrice = 0,
		nMeso,
		nBonus = 0,
		nPeriod = 0,
		nMaplePoint,
		nPriority = 0,
		nOnSale = 0,
		nGender = 0,
		nForcedCatagory = 0,
		nForcedSubCategory = 0,
		nGameWorld = 0,
		nPbCash = 0,
		nPbPoint = 0,
		nPbGift = 0,
		nRefundable = 0,
		nWebShop = 0,
		nModifiedFlag = 0;
};

