#pragma once
#include "FieldObj.h"
#include <mutex>
#include <map>
#include <set>
#include <vector>
#include "TemporaryStat.h"

class ClientSocket;
class OutPacket;
class Field;
class FieldSet;
class Portal;
class InPacket;
class Npc;
class Pet;
class AsyncScheduler;
struct GA_Character;
struct GW_Avatar;
struct GW_FuncKeyMapped;

class BasicStat;
class SecondaryStat;
struct TemporaryStat;
struct AttackInfo;
struct ActItem;
class SkillEntry;
class Summoned;
class Script;
class Trunk;
struct MobSkillLevelData;
class CalcDamage;
class MiniRoomBase;

class User : public FieldObj
{
public:

	//User正在進行的轉換狀態
	//TransferChannel與TransferShop 要附帶SecondaryStat::EncodeInternal以便Center做轉傳
	enum class TransferStatus : unsigned char
	{
		eOnTransferNone = 0x00, //正常 
		eOnTransferField = 0x01, //更換地圖中
		eOnTransferChannel = 0x02, //更換頻道中
		eOnTransferShop = 0x03, //進入商城中
	};

	enum class Message : unsigned char
	{
		eDropPickUpMessage = 0x00,
		eQuestRecordMessage = 0x01,
		//eQuestRecordMessageAddValidCheck = 0x02,
		eCashItemExpireMessage = 0x02,
		eIncEXPMessage = 0x03,
		eIncSPMessage = 0x04,
		eIncPOPMessage = 0x05,
		eIncMoneyMessage = 0x06,
		eIncGPMessage = 0x07,
		eIncCommitmentMessage = 0x08,
		eGiveBuffMessage = 0x09,
		eGeneralItemExpireMessage = 0x0B,
		eSystemMessage = 0x0C,
		eQuestRecordExMessage = 0x0D,
		eWorldShareRecordMessage = 0x0E,
		eItemProtectExpireMessage = 0x0F,
		eItemExpireReplaceMessage = 0x10,
		eItemAbilityTimeLimitedExpireMessage = 0x11,
		eSkillExpireMessage = 0x12,
		eIncNonCombatStatEXPMessage = 0x13,
		eLimitNonCombatStatEXPMessage = 0x14,
		//0x15
		eAndroidMachineHeartAlsetMessage = 0x16,
		eIncFatigueByRestMessage = 0x17,
		eIncPvPPointMessage = 0x18,
		ePvPItemUseMessage = 0x19,
		eWeddingPortalError = 0x1A,
		eIncHardCoreExpMessage = 0x1B,
		eNoticeAutoLineChanged = 0x1C,
		eEntryRecordMessage = 0x1D,
		eEvolvingSystemMessage = 0x1E,
		eEvolvingSystemMessageWithName = 0x1F,
		eCoreInvenOperationMessage = 0x20,
		eNxRecordMessage = 0x21,
		eBlockedBehaviorMessage = 0x22,
		eIncWPMessage = 0x23,
		eMaxWPMessage = 0x24,
		eStylishKillMessage = 0x25,
		eExpiredCashItemResultMessage = 0x26,
		eCollectionRecordMessage = 0x27,
		//0x28
		eQuestExpired = 0x29,
		eWriteMessageInGame = 0x2A,
		eFishSystemBonus = 0x2B,
		eWriteGreenMessageInGame = 0x2C
	};

	enum Effect : unsigned char
	{
		eEffect_LevelUp = 0x00, //Flag only
		eEffect_OnUseSkill = 0x01,
		eEffect_ShowSkillAffected = 0x05, 
		eEffect_ChangeJobEffect = 0x08,//Flag only
		eEffect_QuestCompleteEffect = 0x09, //Flag only
		eEffect_IncDecHpEffect = 0x0A,
	};

private:
	static const int MAX_PET_INDEX = 3;

	std::recursive_mutex m_mtxUserlock, m_scriptLock;
	int m_tLastUpdateTime = 0;

	//Network
	ClientSocket *m_pSocket;
	TransferStatus m_nTransferStatus;

	//Stat
	GA_Character *m_pCharacterData;
	BasicStat* m_pBasicStat;
	SecondaryStat* m_pSecondaryStat;
	GW_FuncKeyMapped *m_pFuncKeyMapped;
	int m_nInvalidDamageMissCount = 0,
		m_nInvalidDamageCount = 0;
	bool m_bDeadlyAttack = false;

	//System
	AsyncScheduler *m_pUpdateTimer;
	Script* m_pScript = nullptr;
	Npc *m_pTradingNpc = nullptr;
	int m_nTrunkTemplateID = 0;
	Trunk *m_pTrunk = nullptr;
	CalcDamage* m_pCalcDamage = nullptr;

	//Pet
	Pet* m_apPet[MAX_PET_INDEX] = { nullptr };

	//Summoned
	std::vector<Summoned*> m_lSummoned;
	std::vector<int> m_aMigrateSummoned;

	//World
	std::set<int> m_snPartyInvitedCharacterID, m_snGuildInvitedCharacterID;

	//Party
	int m_nPartyID = -1, m_nTeamForMCarnival = -1;

	//Guild
	std::string m_sGuildName;
	int m_nMarkBg = 0, m_nMarkBgColor = 0, m_nMark = 0, m_nMarkColor = 0;

	//MiniRoom
	MiniRoomBase* m_pMiniRoom = nullptr;
	bool m_bHasOpenedEntrustedShop = false;

	void TryParsingDamageData(AttackInfo *pInfo, InPacket *iPacket);
	AttackInfo* TryParsingAttackInfo(AttackInfo* pInfo, int nType, InPacket *iPacket);
	AttackInfo* TryParsingMeleeAttack(AttackInfo* pInfo, int nType, InPacket *iPacket);
	AttackInfo* TryParsingMagicAttack(AttackInfo* pInfo, int nType, InPacket *iPacket);
	AttackInfo* TryParsingShootAttack(AttackInfo* pInfo, int nType, InPacket *iPacket);
	AttackInfo* TryParsingBodyAttack(AttackInfo* pInfo, int nType, InPacket *iPacket);

public:

	static User* FindUser(int nUserID);
	static User* FindUserByName(const std::string& strName);

	User(ClientSocket *pSocket, InPacket *iPacket);
	void EncodeCharacterDataInternal(OutPacket *oPacket);
	void FlushCharacterData();
	~User();

	//Basic Routine
	int GetUserID() const;
	int GetChannelID() const;
	int GetAccountID() const;
	const std::string& GetName() const;
	std::recursive_mutex& GetLock();
	void Update();

	GA_Character* GetCharacterData();
	Field* GetField();
	FieldSet* GetFieldSet();
	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
	void SendPacket(OutPacket *oPacket);
	void OnPacket(InPacket *iPacket);
	void LeaveField();
	void OnMigrateIn();
	void OnMigrateOut();
	bool CanAttachAdditionalProcess();

	//TransferStatus
	void SetTransferStatus(TransferStatus e);
	TransferStatus GetTransferStatus() const;

	void OnTransferFieldRequest(InPacket* iPacket);
	bool TryTransferField(int nFieldID, const std::string& sPortalName);
	void OnTransferChannelRequest(InPacket* iPacket);
	void OnMigrateToCashShopRequest(InPacket* iPacket);
	void OnChat(InPacket *iPacket);
	void EncodeChatMessage(OutPacket *oPacket, const std::string strMsg, bool bAdmin, bool bBallon);
	void OnAttack(int nType, InPacket *iPacket);
	void OnHit(InPacket *iPacket);
	void OnLevelUp();
	void OnEmotion(InPacket *iPacket);
	void PostTransferField(int dwFieldID, Portal* pPortal, int bForce);
	void SetMovePosition(int x, int y, char bMoveAction, short nFSN);

	//Avatar
private:
	void UpdateAvatar();

public:
	GW_Avatar* GetAvatar();
	void OnAvatarModified();
	void EncodeAvatar(OutPacket *oPacket);
	void EncodeCharacterData(OutPacket *oPacket);
	void EncodeCoupleInfo(OutPacket *oPacket);
	void EncodeFriendshipInfo(OutPacket *oPacket);
	void EncodeMarriageInfo(OutPacket *oPacket);

	//Stat
	CalcDamage* GetCalcDamage();
	SecondaryStat* GetSecondaryStat();
	BasicStat* GetBasicStat();
	void ValidateStat(bool bCalledByConstructor = false);
	void SendCharacterStat(bool bOnExclRequest, long long int liFlag);
	void SendTemporaryStatReset(TemporaryStat::TS_Flag& flag);
	void SendTemporaryStatSet(TemporaryStat::TS_Flag& flag, int tDelay);
	void ResetTemporaryStat(int tCur, int nReasonID);
	void OnAbilityUpRequest(InPacket *iPacket);
	long long int IncMaxHPAndMP(int nFlag, bool bLevelUp);
	void OnStatChangeByMobSkill(int nSkillID, int nSLV, const MobSkillLevelData* pLevel, int tDelay, int nTemplateID, bool bResetBySkill = false, bool bForcedSetTime = false, int nForcedSetTime = 0);
	void OnStatChangeByMobAttack(int nMobTemplateID, int nMobAttackIdx);

	//Item Use
	void OnStatChangeItemUseRequest(InPacket *iPacket, bool bByPet);
	void OnStatChangeItemCancelRequest(InPacket *iPacket);
	void OnMobSummonItemUseRequest(InPacket *iPacket);

	//Message
	void SendDropPickUpResultPacket(bool bPickedUp, bool bIsMoney, int nItemID, int nCount, bool bOnExcelRequest);
	void SendDropPickUpFailPacket(bool bOnExcelRequest);
	void SendQuestRecordMessage(int nKey, int nState, const std::string& sStringRecord);
	void SendIncEXPMessage(bool bIsLastHit, int nIncEXP, bool bOnQuest, int nIncEXPBySMQ, int nEventPercentage, int nPartyBonusPercentage, int nPlayTimeHour, int nQuestBonusRate, int nQuestBonusRemainCount, int nPartyBonusEventRate, int nWeddingBonusEXP);

	//Npc & Script
	Script* GetScript();
	void SetScript(Script* pScript);
	void OnSelectNpc(InPacket *iPacket);
	void OnScriptMessageAnswer(InPacket *iPacket);
	void SetTradingNpc(Npc *pNpc);
	Npc* GetTradingNpc();

	//Quest
	void OnQuestRequest(InPacket *iPacket);
	void OnAcceptQuest(InPacket *iPacket, int nQuestID, int dwTemplateID, Npc *pNpc);
	void OnCompleteQuest(InPacket *iPacket, int nQuestID, int dwTemplateID, Npc *pNpc, bool bIsAutoComplete);
	void OnResignQuest(InPacket *iPacket, int nQuestID);
	void OnLostQuestItem(InPacket *iPacket, int nQuestID);
	void TryQuestStartAct(int nQuestID, int nNpcID, Npc *pNpc);
	void TryQuestCompleteAct(int nQuestID, Npc *pNpc);
	void TryExchange(const std::vector<ActItem*>& aActItem);
	bool AllowToGetQuestItem(const ActItem* pActionItem);
	void SendQuestResult(int nResult, int nQuestID, int dwTemplateID);

	//Effect
	void SendQuestEndEffect();
	void SendUseSkillEffect(int nSkillID, int nSLV);
	void SendLevelUpEffect();
	void SendChangeJobEffect();

	//Message
	void SendChatMessage(int nType, const std::string& sMsg);
	void SendNoticeMessage(const std::string& sMsg);

	//Func Key Mapped
	void SendFuncKeyMapped();
	void OnFuncKeyMappedModified(InPacket *iPacket);

	//Pet
	void OnPetPacket(InPacket *iPacket);
	void ActivatePet(int nPos, int nRemoveReaseon, bool bOnInitialize);
	int GetMaxPetIndex();
	void OnActivatePetRequest(InPacket *iPacket);

	//Summoned
	void OnSummonedPacket(InPacket *iPacket);
	void ReregisterSummoned();
	void CreateSummoned(const SkillEntry* pSkill, int nSLV, const FieldPoint& pt, bool bMigrate);
	void RemoveSummoned(int nSkillID, int nLeaveType, int nForceRemoveSkillID); //nForceRemoveSkillID = -1 means that remove all summoneds.

	//Party
	void AddPartyInvitedCharacterID(int nCharacterID);
	bool IsPartyInvitedCharacterID(int nCharacterID);
	void RemovePartyInvitedCharacterID(int nCharacterID);
	void ClearPartyInvitedCharacterID();
	void SetPartyID(int nPartyID);
	int GetPartyID() const;
	void PostHPToPartyMembers();
	int GetMCarnivalTeam() const;
	void SetMCarnivalTeam(int nTeam);

	//Guild
	void AddGuildInvitedCharacterID(int nCharacterID);
	const std::set<int>& GetGuildInvitedCharacterID() const;
	void RemoveGuildInvitedCharacterID(int nCharacterID);
	void ClearGuildInvitedCharacterID();
	const std::string& GetGuildName() const;
	void SetGuildName(const std::string& strName);
	void SetGuildMark(int nMarkBg, int nMarkBgColor, int nMark, int nMarkColor);

	//Group & Whisper
	void OnSendGroupMessage(InPacket *iPacket);
	void OnWhisper(InPacket *iPacket);

	//MiniRoom
	MiniRoomBase* GetMiniRoom();
	void SetMiniRoom(MiniRoomBase* pMiniRoom);
	void SetMiniRoomBalloon(bool bOpen);
	void EncodeMiniRoomBalloon(OutPacket *oPacket, bool bOpen);
	bool HasOpenedEntrustedShop() const;
	void SetEntrustedShopOpened(bool bOpened);
	void CreateEmployee(bool bOpen);

	//Trunk
	void OnTrunkResult(InPacket *iPacket);
	void SetTrunk(Trunk *pTrunk);
};

