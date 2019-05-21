#include "Reward.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Wz\ImgAccessor.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "QuestMan.h"
#include "QuestDemand.h"
#include "ItemInfo.h"

std::map<int, std::vector<RewardInfo*>> Reward::stMobRewardInfo;
std::map<int, std::vector<RewardInfo*>> Reward::stReactorRewardInfo;
double Reward::ms_fIncDropRate = 1.0;

Reward::Reward()
{
}

Reward::Reward(Reward * pOther)
{
	m_pItem = pOther->m_pItem;
	m_nMoney = pOther->m_nMoney;
	m_nType = pOther->m_nType;
	m_pInfo = pOther->m_pInfo;
}

Reward::~Reward()
{
}

void Reward::LoadReward()
{
	WZ::ImgAccessor img("./DataSrv/Reward");
	stMobRewardInfo.clear();
	bool bMobReward = false;
	for (auto& mobNode : img)
	{
		bMobReward = mobNode.Name()[0] == 'm';
		int nTemplateID = atoi(mobNode.Name().substr(1, mobNode.Name().size() - 1).c_str());
		unsigned int unTotalWeight = 0;
		for (auto& rewardNode : mobNode)
		{
			RewardInfo* pInfo = AllocObj(RewardInfo);
			for (auto& node : rewardNode)
			{
				if (node.Name() == "prob") {
					std::string strProb = node;
					auto dProb = atof(strProb.substr(4, strProb.size() - 4).c_str()) * 1000000000.0;
					pInfo->m_unWeight = (unsigned int)dProb;
				}
				else if (node.Name() == "money") 
				{
					pInfo->m_nMoney = (int)node;
					pInfo->m_nType = 0;
				}
				else if (node.Name() == "item")
				{
					pInfo->m_nType = 1;
					pInfo->m_nItemID = (int)node;
					int nQuestID = QuestMan::GetInstance()->GetQuestByItem(pInfo->m_nItemID);
					if (nQuestID != 0)
					{
						auto pDemand = QuestMan::GetInstance()->GetCompleteDemand(nQuestID);
						if (pDemand)
						{
							pInfo->m_usQRKey = nQuestID;
							auto& aDemandItem = pDemand->m_mDemandItem;
							for (auto& nItem : aDemandItem)
								if (nItem.first == pInfo->m_nItemID)
									pInfo->m_nMaxCount = nItem.second;
						}
					}
				}
				else if (node.Name() == "min")
					pInfo->m_nMin = (int)node;
				else if (node.Name() == "max")
					pInfo->m_nMax = (int)node;
				else if (node.Name() == "period")
					pInfo->m_nPeriod = (int)node;
				else if (node.Name() == "premium")
					pInfo->m_bPremiumMap = ((int)node == 1);
			}

			if (bMobReward)
				stMobRewardInfo[nTemplateID].push_back(pInfo);
			else
				stReactorRewardInfo[nTemplateID].push_back(pInfo);
		}
	}
}

const std::vector<RewardInfo*>* Reward::GetMobReward(int nTemplateID)
{
	static std::vector<RewardInfo*> aEmpty;
	auto findIter = stMobRewardInfo.find(nTemplateID);
	if (findIter == stMobRewardInfo.end())
		return &aEmpty;
	return &(findIter->second);
}

const std::vector<RewardInfo*>* Reward::GetReactorReward(int nTemplateID)
{
	static std::vector<RewardInfo*> aEmpty;
	auto findIter = stReactorRewardInfo.find(nTemplateID);
	if (findIter == stReactorRewardInfo.end())
		return &aEmpty;
	return &(findIter->second);
}

std::vector<Reward*> Reward::Create(const std::vector<RewardInfo*>* aRewardInfo, bool bPremiumMap, double dRegionalIncRate, double dShowdown, double dOwnerDropRate, double dOwnerDropRate_Ticket, double* pRewardRate)
{
	std::vector<Reward*> aRet;
	double dDropRate = 1.0,
		dRewardRate = 1.0,
		dMoneyRate = 1.0,
		dItemRate = 1.0;

	int nDiff = 0, 
		nRange = 0;

	unsigned int unRndBase = 0, 
		unRndRes = 0;

	Reward *pReward = nullptr;
	for (auto& pInfo : (*aRewardInfo))
	{
		dRewardRate = 1.0;
		dMoneyRate = 1.0;
		dItemRate = 1.0;
		if (pRewardRate && pInfo->m_nType == 1)
		{
			int nTI = pInfo->m_nItemID / 1000000;
			if (nTI != GW_ItemSlotBase::EQUIP && nTI != GW_ItemSlotBase::CASH)
			{
				auto pItem = ItemInfo::GetInstance()->GetBundleItem(pInfo->m_nItemID);
				if (pItem && pItem->nMCType >= 0)
					dRewardRate = pRewardRate[pItem->nMCType];
			}
		}
		if (pInfo->m_nType == 0)
			dMoneyRate = dOwnerDropRate_Ticket;
		else if (pInfo->m_nType == 1)
			dItemRate = dOwnerDropRate;

		unRndBase = (unsigned int)(1000000000 / (ms_fIncDropRate * dDropRate) / dRegionalIncRate / dShowdown / dOwnerDropRate / dItemRate / dRewardRate);
		unRndRes = 0;

		if (unRndBase)
			unRndRes = ((unsigned int)Rand32::GetInstance()->Random()) % unRndBase;
		else
			unRndRes = ((unsigned int)Rand32::GetInstance()->Random());

		if ((unsigned int)unRndRes < pInfo->m_unWeight)
		{
			//Determine the drop number.
			nDiff = pInfo->m_nMax - pInfo->m_nMin;
			nRange = pInfo->m_nMin + (nDiff == 0 ? 0 : ((unsigned int)Rand32::GetInstance()->Random()) % nDiff);

			//Create Reward
			pReward = AllocObj(Reward);
			if (pInfo->m_nItemID == 0)
			{
				nRange = 1 + (Rand32::GetInstance()->Random() % (unsigned int)pInfo->m_nMoney);
				pReward->SetMoney(nRange);
			}
			else
			{
				auto pItem = ItemInfo::GetInstance()->GetItemSlot(pInfo->m_nItemID, ItemInfo::ItemVariationOption::ITEMVARIATION_NORMAL);
				if (!pItem)
				{
					FreeObj(pReward);
					continue;
				}
				pReward->SetItem(pItem);
				if (pInfo->m_nItemID / 1000000 != GW_ItemSlotBase::EQUIP)
					((GW_ItemSlotBundle*)pItem)->nNumber = nRange;
			}
			pReward->SetType(1);
			pReward->SetPeriod(pInfo->m_nPeriod);
			aRet.push_back(pReward);
		}

		/*int nRetCount = (int)aRet.size();
		for (int i = 0; i < nRetCount; ++i)
			std::swap(aRet[(int)Rand32::GetInstance()->Random() % nRetCount],
				aRet[(int)Rand32::GetInstance()->Random() % nRetCount]);*/
	}
	return aRet;
}

