#pragma once
#include <vector>

struct GW_ItemSlotBase;

class TrunkDBAccessor
{
public:
	static std::pair<int, int> LoadTrunk(int nAccountID);
	static void UpdateTrunk(int nAccountID, int nMoney, int nSlotCount);
	static std::vector<GW_ItemSlotBase*> LoadTrunkEquip(int nAccountID);
	static std::vector<GW_ItemSlotBase*> LoadTrunkEtc(int nAccountID); 
	static std::vector<GW_ItemSlotBase*> LoadTrunkInstall(int nAccountID);
	static std::vector<GW_ItemSlotBase*> LoadTrunkConsume(int nAccountID);

	static void MoveSlotToTrunk(int nAccountID, long long int liItemSN, int nTI);
	static void MoveTrunkToSlot(int nAccountID, long long int liItemSN, int nTI, bool bTreatSingly);
};

