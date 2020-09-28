#pragma once
#include "GW_ItemSlotBase.h"

struct GW_ItemSlotBundle :
	public GW_ItemSlotBase
{
	int nNumber = 0;

public:
	GW_ItemSlotBundle();
	~GW_ItemSlotBundle();

	static void LoadAll(int nType, int nCharacterID, std::map<int, ZSharedPtr<GW_ItemSlotBase>>& mRes);
	void Load(ATOMIC_COUNT_TYPE SN);
	void Save(int nCharacterID, bool bRemoveRecord = false, bool bExpired = false);

	void Encode(OutPacket *oPacket, bool bForInternal) const;
	void RawEncode(OutPacket *oPacket) const;

	void Decode(InPacket *iPacket, bool bForInternal);
	void RawDecode(InPacket *iPacket); 

	GW_ItemSlotBase * MakeClone() const;
};

