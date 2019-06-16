#pragma once
#include <atomic>
#include <map>
#include "..\WvsLib\Memory\ZMemory.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

struct GW_ItemSlotBundle;
struct GW_ItemSlotEquip;
struct GW_ItemSlotPet;

struct GW_ItemSlotBase
{
	typedef unsigned long long int ATOMIC_COUNT_TYPE;
	static const int LOCK_POS = 32767;

	enum GW_ItemSlotType
	{
		EQUIP = 1,
		CONSUME,
		INSTALL,
		ETC,
		CASH
	};

	enum GW_ItemSlotInstanceType
	{
		GW_ItemSlotEquip_Type = 1,
		GW_ItemSlotBundle_Type = 2,
		GW_ItemSlotPet_Type = 3
	};

	static ATOMIC_COUNT_TYPE InitItemSN(GW_ItemSlotType type);
	static ATOMIC_COUNT_TYPE GetInitItemSN(GW_ItemSlotType type, int nWorldID, int nChannelID);
	static ATOMIC_COUNT_TYPE IncItemSN(GW_ItemSlotType type);

	static std::atomic<ATOMIC_COUNT_TYPE> ms_liSN[6];
	static int ms_nChannelID, ms_nWorldID;
	static void SetInitSN(int nTI, ATOMIC_COUNT_TYPE liSN);
	static ATOMIC_COUNT_TYPE GetNextSN(int nTI);

	GW_ItemSlotType nType;
	short nPOS = 0;

	int nItemID = 0,
		nAttribute = 0,
		nCharacterID = -1;

	long long int liExpireDate = GameDateTime::TIME_UNLIMITED,
				  liItemSN = -1,
				  liCashItemSN = -1;

	bool bIsPet = false, bIsCash =  false;
	unsigned char nInstanceType = 0;

public:
	GW_ItemSlotBase();
	virtual ~GW_ItemSlotBase();

	bool IsProtectedItem() const;
	virtual void Encode(OutPacket *oPacket, bool bForInternal) const = 0;
	virtual void RawEncode(OutPacket *oPacket) const = 0;
	void DecodeInventoryPosition(InPacket *iPacket);
	virtual void Decode(InPacket *iPacket, bool bForInternal) = 0;
	virtual void RawDecode(InPacket *iPacket) = 0;
	void EncodeInventoryPosition(OutPacket *oPacket) const;
	void EncodeTradingPosition(OutPacket *oPacket) const;

	virtual GW_ItemSlotBase* MakeClone() const = 0;

	static void LoadAll(int nType, int nCharacterID, bool bIsCash, bool bIsPet, std::map<int, ZSharedPtr<GW_ItemSlotBase>>& mRes);
	virtual void Load(ATOMIC_COUNT_TYPE SN) = 0;
	virtual void Save(int nCharacterID, bool bRemoveRecord = false) = 0;

	static GW_ItemSlotBase* CreateItem(int nIntanceType);
};
