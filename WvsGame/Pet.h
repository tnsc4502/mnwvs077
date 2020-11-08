#pragma once
#include "FieldObj.h"
#include "..\WvsLib\Memory\ZMemory.h"

struct GW_ItemSlotBase;
struct GW_ItemSlotPet;
class PetTemplate;
class InPacket;
class Field;

class Pet : public FieldObj
{
	friend class User;

	const PetTemplate *m_pTemplate;
	User *m_pOwner;
	Field *m_pField;
	ZSharedPtr<GW_ItemSlotBase> m_pPetSlot;
	unsigned char m_nIndex = 0, m_nRemoveReason = 0, m_nOvereat = 0;
	unsigned int m_tRemainHungriness = 0, m_tLastUpdated = 0;

public:
	Pet(ZSharedPtr<GW_ItemSlotBase>& pPetSlot);
	~Pet();

	void SetIndex(unsigned char nIndex);
	unsigned char GetIndex() const;

	void Init(User *pUser);
	void OnPacket(InPacket *iPacket, int nType);
	void OnEnterField(Field *pField);
	void OnLeaveField(int nRemoveReason = 0);
	void OnMove(InPacket *iPacket);
	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
	void EncodeInitData(OutPacket *oPacket);
	void Update(unsigned int tCur, bool& bRemove);
	void OnEatFood(int niRepleteness);
	int GetLevel();
	int IncTameness(int nInc);
	const GW_ItemSlotPet *GetItemSlot() const;
};

