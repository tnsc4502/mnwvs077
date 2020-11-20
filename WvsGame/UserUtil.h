#pragma once
class UserUtil
{
public:
	static long m_nEXP[251];

	static void LoadEXP();
	static bool IsBeginnerJob(int nJob);
	static int GetJobLevel(int nJob);

	static bool IsAdventurerWarrior(int nJob);
	static bool IsAdventurerMagic(int nJob);
	static bool IsAdventurerArcher(int nJob);
	static bool IsAdventurerThief(int nJob);
	static bool IsAdventurerPirate(int nJob);
};

