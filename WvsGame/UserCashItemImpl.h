/*
09/12/2019 added.
Implementations of cash item usages.
*/

#pragma once

class User;
class InPacket;
class OutPacket;

class UserCashItemImpl
{
public:
	enum MapTransferResult
	{
		eMapTransfer_RecordedDeleted = 2,
		eMapTransfer_SuccessfullyRecorded = 3,
		eMapTransfer_InValidFieldID = 5,
		eMapTransfer_InValidTargetUser = 6,
		eMapTransfer_InValidUserStat = 7,
		eMapTransfer_InValidFieldStat = 8,
		eMapTransfer_TransferOnIdenticalField = 9,
		eMapTransfer_ListIsFull = 10,
		eMapTransfer_LevelNotSatisfied = 11,
	};

	static int ConsumeSpeakerChannel(User *pUser, InPacket *iPacket);
	static int ConsumeSpeakerWorld(User *pUser, int nCashItemType, InPacket *iPacket);
	static int ConsumeAvatarMegaphone(User *pUser, int nItemID, InPacket *iPacket);
	static int ConsumePetSkill(User *pUser, int nItemID, InPacket *iPacket);
	static int ConsumeStatChange(User *pUser, InPacket *iPacket);
	static int ConsumeSkillChange(User *pUser, int nItemID, InPacket *iPacket);
	static int ConsumeSetItemName(User *pUser, InPacket *iPacket);
	static int ConsumeADBoard(User *pUser, InPacket *iPacket);
	static int ConsumeItemProtector(User *pUser, InPacket *iPacket);
	static int ConsumeWeatherItem(User *pUser, int nItemID, InPacket *iPacket);
	static int ConsumeJukeBox(User *pUser, int nItemID, InPacket *iPacket);
	static int ConsumeSendMemo(User *pUser, int nPOS, InPacket *iPacket);
	static int ConsumeShopScanner(User *pUser, int nPOS, InPacket *iPacket);
	static int ConsumeMapTransfer(User *pUser, int nItemID, InPacket *iPacket);
	static bool IsMapTransferAvailable(User *pUser, int nFieldID, User *pTarget, bool bCanTransferContinent);
};

