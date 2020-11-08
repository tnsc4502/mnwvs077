#include "PetTemplate.h"
#include "ItemInfo.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\ZMemory.h"
#include "..\WvsLib\String\StringUtility.h"

std::map<int, PetTemplate*> PetTemplate::ms_mTemplate;

PetTemplate::PetTemplate()
{
}


PetTemplate::~PetTemplate()
{
}

void PetTemplate::Load()
{
	auto& wzPetFolder = WzResMan::GetInstance()->GetWz(Wz::Item)["Pet"];
	for (auto& imgPet : wzPetFolder)
		RegisterPet(atoi(imgPet.GetName().c_str()), &imgPet);
}

void PetTemplate::RegisterPet(int nTemplateID, void * pData)
{
	WzIterator& imgPet = *(WzIterator*)pData;
	PetTemplate* pTemplate = AllocObj(PetTemplate);
	pTemplate->nTemplateID = nTemplateID;
	pTemplate->nHungry = imgPet["info"]["hungry"];
	pTemplate->nLife = imgPet["info"]["life"];
	pTemplate->bSweepForDrop = ((int)imgPet["info"]["sweepForDrop"] == 1);
	pTemplate->bConsumeHP = ((int)imgPet["info"]["consumeHP"] == 1);
	pTemplate->bConsumeMP = ((int)imgPet["info"]["consumeMP"] == 1);
	pTemplate->bPickUpItem = ((int)imgPet["info"]["pickupItem"] == 1);
	pTemplate->bLongRange = ((int)imgPet["info"]["longRange"] == 1);
	pTemplate->bPickUpAll = ((int)imgPet["info"]["pickupAll"] == 1);
	pTemplate->bCanEvol = ((int)imgPet["info"]["evol"] == 1);

	if (pTemplate->bCanEvol)
	{
		pTemplate->nEvolItemID = imgPet["info"]["evolReqItemID"];
		pTemplate->nEvolReqPetLvl = imgPet["info"]["evolReqPetLvl"];

		int nEvolNo = imgPet["info"]["evolNo"];
		for (int i = 1; i <= nEvolNo; ++i)
		{
			PetEvolData evolData;
			evolData.nTemplateID = imgPet["info"]["evol" + std::to_string(i)];
			evolData.fProb = (float)(int)imgPet["info"]["evolProb" + std::to_string(i)];
			pTemplate->fEvolProbSum += evolData.fProb;
			pTemplate->aPetEvolData.push_back(evolData);
		}
	}

	pTemplate->sName = ItemInfo::GetInstance()->GetItemName(nTemplateID);

	//Load Interactions
	auto& wzInteract = imgPet["interact"];
	for (auto& imgInteract : wzInteract)
	{
		Interaction interaction;
		StringUtility::Split(imgInteract["command"], interaction.asCommand, "|");
		interaction.nFriendnessInc = imgInteract["inc"];
		interaction.nProb = imgInteract["prob"];
		interaction.rgLevel = { imgInteract["l0"], imgInteract["l1"] };

		for (auto& imgSuccess : imgInteract["success"])
		{
			Action action;
			action.sAction = imgSuccess["act"];
			for (auto& imgAction : imgSuccess)
			{
				if (imgAction.GetName() == "act")
					continue;
				action.asSpeak.push_back(imgAction);
			}
			interaction.actSuccess.push_back(action);
		}

		for (auto& imgFail : imgInteract["fail"])
		{
			Action action;
			action.sAction = imgFail["act"];
			for (auto& imgAction : imgFail)
			{
				if (imgAction.GetName() == "act")
					continue;
				action.asSpeak.push_back(imgAction);
			}
			interaction.actFail.push_back(action);
		}
		pTemplate->aInteraction.push_back(interaction);
	}

	//Load Food Reactions
	auto& wzFoodReaction = imgPet["food"];
	for (auto& imgFoodReaction : wzFoodReaction)
	{
		FoodReaction foodReaction;
		foodReaction.rgLevel = { imgFoodReaction["l0"], imgFoodReaction["l1"] };

		for (auto& imgSuccess : imgFoodReaction["success"])
		{
			Action action;
			action.sAction = imgSuccess["act"];
			for (auto& imgAction : imgSuccess)
			{
				if (imgAction.GetName() == "act")
					continue;
				action.asSpeak.push_back(imgAction);
			}
			foodReaction.actSuccess.push_back(action);
		}

		for (auto& imgFail : imgFoodReaction["fail"])
		{
			Action action;
			action.sAction = imgFail["act"];
			for (auto& imgAction : imgFail)
			{
				if (imgAction.GetName() == "act")
					continue;
				action.asSpeak.push_back(imgAction);
			}
			foodReaction.actFail.push_back(action);
		}
		pTemplate->aFoodReaction.push_back(foodReaction);
	}

	ms_mTemplate.insert({ nTemplateID, pTemplate });
}

const PetTemplate * PetTemplate::GetPetTemplate(int nTemplateID)
{
	auto findIter = ms_mTemplate.find(nTemplateID);
	return findIter == ms_mTemplate.end() ? nullptr : findIter->second;
}

const PetTemplate * PetTemplate::GetRandEvolPetTemplate() const
{
	if (fEvolProbSum == 0)
		return nullptr;

	int nRnd = ((unsigned int)Rand32::GetInstance()->Random()) % (int)fEvolProbSum;
	int nRndSum = 0;
	for (auto& evolData : aPetEvolData)
	{
		if (nRndSum <= nRnd && nRnd <= (int)evolData.fProb + nRndSum)
			return GetPetTemplate(evolData.nTemplateID);

		nRndSum += (int)evolData.fProb;
	}
	return nullptr;
}
