#pragma once
#include <map>
#include <vector>
#include <string>
#include "..\WvsLib\Common\CommonDef.h"

class PetTemplate
{
	ALLOW_PRIVATE_ALLOC
public:
	enum PetAbility
	{
		//e_PetAbility_PickupMeso = 0x1,
		e_PetSkill_PickupItem = 0x1,
		e_PetSkill_LongRange = 0x2,
		e_PetSkill_PickupSweepForDrop = 0x04,
		e_PetSkill_IgnorePickup = 0x08,
		e_PetSkill_PickupAll = 0x10,
		e_PetAbility_ConsumeHP = 0x20,
		e_PetAbility_ConsumeMP = 0x40,
		//e_PetAbility_AutoBuff = 0x80,
		e_PetAbility_AutoSpeaking = 0x100
	};

	struct Action
	{
		std::string sAction;
		std::vector<std::string> asSpeak;
	};

	struct Interaction
	{
		std::pair<int, int> rgLevel; //<low, high>
		int nFriendnessInc, nProb;
		std::vector<std::string> asCommand;
		std::vector<Action> actSuccess, actFail;
	};

	struct FoodReaction
	{
		std::pair<int, int> rgLevel; //<low, high>
		std::vector<Action> actSuccess, actFail;
	};

	struct PetEvolData
	{
		int nTemplateID = 0;
		float fProb = 0;
	};

	int nTemplateID = 0, 
		nHungry = 0, 
		nLife = 0,
		nMoveAbility = 0,
		nEvolItemID = 0,
		nEvolReqPetLvl = 0,
		nEvolReqTameness = 0;

	bool bPickUpItem = false,
		bPickUpAll = false,
		bConsumeHP = false,
		bConsumeMP = false,
		bSweepForDrop = false,
		bLongRange = false,
		bCanEvol = false;

	float fEvolProbSum = 0;

	std::vector<Interaction> aInteraction;
	std::vector<FoodReaction> aFoodReaction;
	std::vector<PetEvolData> aPetEvolData;
	std::vector<std::string> asAction;

	std::string sName;

private:
	PetTemplate();
	~PetTemplate();
	static std::map<int, PetTemplate*> ms_mTemplate;

public:
	static void Load();
	static void RegisterPet(int nTemplateID, void *pData);
	static const PetTemplate* GetPetTemplate(int nTemplateID);
	const PetTemplate* GetRandEvolPetTemplate() const;
};

