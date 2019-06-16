#pragma once
#include "FieldObj.h"
#include "..\WvsLib\Memory\ZMemory.h"

struct GW_ItemSlotBase;
struct GW_ItemSlotPet;
class InPacket;
class Field;

class Pet : public FieldObj
{
	friend class User;

	User *m_pOwner;
	Field *m_pField;
	ZSharedPtr<GW_ItemSlotBase> m_pPetSlot;
	unsigned char m_nIndex = 0;

public:
	Pet(ZSharedPtr<GW_ItemSlotBase>& pPetSlot);
	~Pet();

	void SetIndex(unsigned char nIndex);
	unsigned char GetIndex() const;

	void Init(User *pUser);
	void OnPacket(InPacket *iPacket, int nType);
	void OnEnterField(Field *pField);
	void OnLeaveField();
	void OnMove(InPacket *iPacket);
	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
	void EncodeInitData(OutPacket *oPacket);
	const GW_ItemSlotPet *GetItemSlot() const;
};

