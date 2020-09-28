#pragma once
class UtilUser
{
public:
	static long m_nEXP[251];

	static void LoadEXP();
	static bool IsBeginnerJob(int nJob);
	static int GetJobLevel(int nJob);

	static bool IsAdventurerWarrior(int nJob);
	static bool IsAdventurerMage(int nJob);
	static bool IsAdventurerArchor(int nJob);
	static bool IsAdventurerRogue(int nJob);
	static bool IsAdventurerPirate(int nJob);
};

