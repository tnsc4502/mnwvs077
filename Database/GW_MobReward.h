#pragma once
#include <vector>
#include <map>
#include "..\WvsLib\Common\CommonDef.h"

class GW_MobReward
{
	ALLOW_PRIVATE_ALLOC

	GW_MobReward();
	~GW_MobReward();
	struct RewardInfo
	{
		int nItemID = 0,
			nMoney = 0,
			nCountMin = 1,
			nCountMax = 1,
			nWeight = 0;

		short nQRecord = 0;

		long long int liExpired;
		bool bPremiumMap = false;
	};

	//給Manager用
	static std::map<int, std::vector<RewardInfo*>> m_mReward;

	//每個怪物自己持有，指向m_mReward中
	std::vector<RewardInfo*> m_aReward;

	int m_nTotalWeight = 0;

public:
	static GW_MobReward* GetInstance();
	void Load();
	GW_MobReward* GetMobReward(int nMobID);
	const std::vector<RewardInfo*>& GetRewardList() const;
	const int GetTotalWeight() const;
};

