#pragma once
#include "WvsUnified.h"

class SocketBase;

class CharacterDBAccessor
{
private:

public:

	enum CharacterBodyEquipPosition
	{
		eBodyEquip_POS_Cap = -1,
		eBodyEquip_POS_Coat = -5,
		eBodyEquip_POS_Pants = -6,
		eBodyEquip_POS_Shoes = -7,
		eBodyEquip_POS_Weapon = -11,
		eBodyEquip_POS_Cape = -9,
		eBodyEquip_POS_Shield = -10,
	};

	enum NewCharacterEquipDataPosition
	{
		eEQPData_POS_CapEquip = 0,
		eEQPData_POS_CoatEquip = 1,
		eEQPData_POS_PantsEquip = 2,
		eEQPData_POS_ShoesEquip = 3,
		eEQPData_POS_WeaponEquip = 4,
		eEQPData_POS_CapeEquip = 5,
		eEQPData_POS_ShieldEquip = 6,
		eEQPData_POS_END,
	};

	enum NewCharacterStatDataPosition
	{
		eStatData_POS_Str = 0,
		eStatData_POS_Dex = 1,
		eStatData_POS_Int = 2,
		eStatData_POS_Luk = 3,
		eStatData_POS_HP = 4,
		eStatData_POS_MP = 5,
		eStatData_POS_MaxHP = 6,
		eStatData_POS_MaxMP = 7,
		eStatData_POS_SP = 8,
		eStatData_POS_AP = 9,
		eStatData_POS_Job = 10,
		eStatData_POS_SubJob = 11,
		eStatData_POS_Level = 12,
		eStatData_POS_END = 13,
	};

	static CharacterDBAccessor* GetInstance()
	{
		static CharacterDBAccessor* sPtrAccessor = new CharacterDBAccessor();
		return sPtrAccessor;
	}

	static void GetDefaultCharacterStat(int *aStat);

	std::vector<int> PostLoadCharacterListRequest(SocketBase *pSrv, int uLocalSocketSN, int nAccountID, int nWorldID);
	void PostCheckDuplicatedID(SocketBase *pSrv, int uLocalSocketSN, int nAccountID, const std::string& sCharacterName);
	void PostCreateNewCharacterRequest(SocketBase *pSrv, int uLocalSocketSN, int nAccountID, int nWorldID, const std::string& strName, int nGender, int nFace, int nHair, int nSkin, const int* aBody, const int* aStat);
	void PostCharacterDataRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *oPacket);
	void PostBuyCashItemRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket);
	void PostLoadLockerRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket);
	void PostUpdateCashRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket);
	void PostMoveSlotToLockerRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket);
	void PostMoveLockerToSlotRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket);
	void PostExpireCashItemRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket);
	int QueryCharacterIDByName(const std::string& strName);
	int QueryCharacterFriendMax(int nCharacterID);
	int QueryCharacterAccountID(int nCharacterID);
	//void PostChargeCashRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket);
	void OnCharacterSaveRequest(void *iPacket);
};

