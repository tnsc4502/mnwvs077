#pragma once
#include "FieldObj.h"
#include "UserUtil.h"
#include <mutex>
#include <unordered_map>
#include <map>
#include <set>
#include <vector>
#include "TemporaryStat.h"
#include "..\WvsLib\Memory\ZMemory.h"

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
struct GW_Memo;
struct GW_WishList;

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
class StoreBank;

class User : public FieldObj
{
public:

	enum class TransferStatus : unsigned char
	{
		eOnTransferNone = 0x00,
		eOnTransferField = 0x01,
		eOnTransferChannel = 0x02,
		eOnTransferShop = 0x03,
	};

	enum class Message : unsigned char
	{
		eDropPickUpMessage = 0x00,
		eQuestRecordMessage = 0x01,
		//eQuestRecordMessageAddValidCheck = 0x02,
		eCashItemExpireMessage = 0x02,
		eIncEXPMessage = 0x03,
		//eIncSPMessage = 0x04,
		eIncPOPMessage = 0x04,
		eIncMoneyMessage = 0x05,
		eIncGPMessage = 0x06,
		eIncCommitmentMessage = 0x07,
		eGiveBuffMessage = 0x08,
		eGeneralItemExpireMessage = 0x09,
		eSystemMessage = 0x0A,
		/*eQuestRecordExMessage = 0x0D,
		eWorldShareRecordMessage = 0x0E,
		eItemProtectExpireMessage = 0x0F,
		eItemExpireReplaceMessage = 0x10,
		eItemAbilityTimeLimitedExpireMessage = 0x11,*/
	};

	enum Effect : unsigned char
	{
		eEffect_LevelUp = 0x00, //Flag only
		eEffect_OnUseSkill = 0x01,
		eEffect_OnSkillAppliedByParty = 0x02,
		eEffect_ShowSkillAffected = 0x05,
		eEffect_PlayPortalSE = 0x07,//Flag only
		eEffect_ChangeJobEffect = 0x08,//Flag only
		eEffect_QuestCompleteEffect = 0x09, //Flag only
		eEffect_IncDecHpEffect = 0x0A,
	};

	enum UserGrade : unsigned char
	{
		eGrade_None = 0,
		eGrade_GM = 1,
		eGrade_SuperGM = eGrade_GM | 2,
		eGrade_SrvAdmin = eGrade_SuperGM | 4
	};

	enum GivePopluarityMessage : unsigned char
	{
		eUserDoesNotExist = 1,
		eLevelIsNotSatisfied = 2,
		eNotAllowed_Day = 3,
		eNotAllowed_Month = 4,
		eUserPOPIsIncreased = 5,
	};

private:
	static const int MAX_PET_INDEX = 3;
	std::recursive_mutex m_mtxUserLock, m_scriptLock;

	unsigned int m_tMigrateTime = 0,
		m_tLastUpdateTime = 0,
		m_tLastBackupTime = 0, 
		m_tLastAliveCheckTime = 0;

	//Network
	ClientSocket *m_pSocket;
	TransferStatus m_nTransferStatus;

	//Stat
	ZUniquePtr<GA_Character> m_pCharacterData;
	ZUniquePtr<BasicStat> m_pBasicStat;
	ZUniquePtr<SecondaryStat> m_pSecondaryStat;
	ZUniquePtr<GW_FuncKeyMapped> m_pFuncKeyMapped;
	int m_nInvalidDamageMissCount = 0,
		m_nInvalidDamageCount = 0,
		m_nActivePortableChairID = 0,
		m_nTargetPosition_X = 0,
		m_nTargetPosition_Y = 0;

	unsigned int 
		m_tLastRecoveryTime = 0,
		m_tLastAliveCheckRequestTime = 0,
		m_tLastAliveCheckRespondTime = 0,
		m_tPortableChairSittingTime = 0,
		m_tNextCheckCashItemExpire = 0;

	unsigned char m_nGradeCode = UserGrade::eGrade_None;

	bool 
		m_bDeadlyAttack = false,
		m_bChase = false;

	//System
	ZUniquePtr<AsyncScheduler> m_pUpdateTimer;
	Script* m_pScript = nullptr;
	ZUniquePtr<StoreBank> m_pStoreBank = nullptr;
	Npc *m_pTradingNpc = nullptr;
	ZUniquePtr<Trunk> m_pTrunk;
	ZUniquePtr<CalcDamage> m_pCalcDamage;
	int m_nTrunkTemplateID = 0, m_nStoreBankTemplateID = 0;

	//ADBoard
	std::string m_sADBoard;
	bool m_bADBoard = false;

	//Pet
	ZUniquePtr<Pet> m_apPet[MAX_PET_INDEX] = { nullptr };

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

	//Cash Sys
	std::unordered_map<int, ZUniquePtr<GW_Memo>> m_mMemo;
	ZUniquePtr<GW_WishList> m_pWishList;

	//TownPortal
	int m_nTownPortalFieldID = 999999999;
	FieldPoint m_ptFieldPortal;

	AttackInfo* TryParsingAttackInfo(AttackInfo* pInfo, int nType, InPacket *iPacket);

	//Check routines
	void CheckCashItemExpire(unsigned int tCur);

public:
	static void TryParsingDamageData(AttackInfo *pInfo, InPacket *iPacket, int nDamageMobCount, int nDamagedCountPerMob);
	static ZSharedPtr<User> FindUser(int nUserID);
	static ZSharedPtr<User> FindUserByName(const std::string& strName);
	static void Broadcast(OutPacket *oPacket);

	User(ClientSocket *pSocket, InPacket *iPacket);
	void EncodeCharacterDataInternal(OutPacket *oPacket);
	void FlushCharacterData();
	~User();

	//Basic Routine
	int GetSocketID() const;
	void CloseSocket();
	int GetUserID() const;
	int GetChannelID() const;
	int GetAccountID() const;
	const std::string& GetName() const;
	std::recursive_mutex& GetLock();
	void Update();
	bool IsShowTo(FieldObj *pUser);

	ZUniquePtr<GA_Character>& GetCharacterData();
	Field* GetField();
	FieldSet* GetFieldSet();
	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
	void SendPacket(OutPacket *oPacket);
	void OnCenterPacket(int nType, InPacket *iPacket);
	void OnPacket(InPacket *iPacket);
	void LeaveField();
	void OnMigrateIn();
	void OnMigrateOut();
	void SendSetFieldPacket(bool bCharacterData);
	bool CanAttachAdditionalProcess();
	void OnWorldQueryResult(InPacket *iPacket);

	//TransferStatus
	void SetTransferStatus(TransferStatus e);
	TransferStatus GetTransferStatus() const;

	void OnTransferFieldRequest(InPacket* iPacket);
	bool TryTransferField(int nFieldID, const std::string& sPortalName, const FieldPoint* pTownPortalPos = nullptr);
	void OnPortalScriptRequest(InPacket *iPacket);
	void OnTransferChannelRequest(InPacket* iPacket);
	void OnMigrateToCashShopRequest(InPacket* iPacket);
	void OnChat(InPacket *iPacket);
	void EncodeChatMessage(OutPacket *oPacket, const std::string strMsg, bool bAdmin, bool bBallon);
	void OnAttack(int nType, InPacket *iPacket);
	void OnHit(InPacket *iPacket);
	void OnLevelUp();
	void OnEmotion(InPacket *iPacket);
	void PostTransferField(int dwFieldID, Portal* pPortal, int bForce);
	void PostPortalTeleport(int nPortalID);
	void SetMovePosition(int x, int y, char bMoveAction, short nFSN);

private:
	//Inspection
	bool InspectDamageMiss(int nDamage, void *pMobAttackInfo, void *pMob, unsigned int *aDamageRandom);

	//Avatar
	void UpdateAvatar();

public:
	GW_Avatar* GetAvatar();
	void OnAvatarModified();
	void EncodeAvatar(OutPacket *oPacket);
	void EncodeCharacterData(OutPacket *oPacket);
	void EncodeCoupleInfo(OutPacket *oPacket);
	void EncodeFriendshipInfo(OutPacket *oPacket);
	void EncodeMarriageInfo(OutPacket *oPacket);
	void DecodeInternal(InPacket *iPacket);
	void EncodeInternal(OutPacket *oPacket);

	//Stat
	ZUniquePtr<CalcDamage>& GetCalcDamage();
	ZUniquePtr<SecondaryStat>& GetSecondaryStat();
	ZUniquePtr<BasicStat>& GetBasicStat();
	void OnUserDead(bool bTown);
	void DecreaseEXP(bool bTown);
	void ValidateStat(bool bCalledByConstructor = false);
	void ValidateEffectItem();
	void SendCharacterStat(bool bOnExclRequest, long long int liFlag);
	void SendPortableChairEffect(int nSeatID);
	void SendTemporaryStatReset(TemporaryStat::TS_Flag& flag);
	void SendTemporaryStatSet(TemporaryStat::TS_Flag& flag, int tDelay);
	void ResetTemporaryStat(int tCur, int nReasonID);
	void OnAbilityUpRequest(InPacket *iPacket);
	long long int IncMaxHPAndMP(int nFlag, bool bLevelUp);
	void OnStatChangeByMobSkill(int nSkillID, int nSLV, const MobSkillLevelData* pLevel, int tDelay, int nTemplateID, bool bResetBySkill = false, bool bForcedSetTime = false, unsigned int nForcedSetTime = 0);
	void OnStatChangeByMobAttack(int nMobTemplateID, int nMobAttackIdx);
	void OnSitRequest(InPacket *iPacket);
	void OnPortableChairSitRequest(InPacket *iPacket);
	void OnSetActiveEffectItem(InPacket *iPacket);
	void OnChangeStatRequest(InPacket *iPacket);
	bool IsAbleToLearnSkillByItem(void *pItem, bool &bSucceed, int &nTargetSkill);
	void OnDropMoneyRequest(InPacket *iPacket);
	void OnCharacterInfoRequest(InPacket *iPacket);
	unsigned char GetGradeCode() const;
	void SetSkillCooltime(int nReason, int tDuration);
	unsigned int GetSkillCooltime(int nReason);
	void SendSkillCooltimeSet(int nReason, unsigned int tTime);
	void OnGivePopularityRequest(InPacket *iPacket);

	//Item Use
	void OnStatChangeItemUseRequest(InPacket *iPacket, bool bByPet);
	void OnStatChangeItemCancelRequest(InPacket *iPacket);
	void OnMobSummonItemUseRequest(InPacket *iPacket);
	void OnSkillLearnItemUseRequest(InPacket *iPacket);
	void SendSkillLearnItemResult(int nItemID, int nTargetSkill, int nMasterLevel, bool bItemUsed, bool bSucceed);
	void OnPortalScrollUseRequest(InPacket *iPacket);
	void OnConsumeCashItemUseRequest(InPacket *iPacket);
	void SetADBoard(const std::string& sADBoard);
	void OnShopScannerResult(InPacket *iPacket);
	void OnMapTransferItemRequest(InPacket *iPacket);
	void SendMapTransferItemResult(int nResultType, bool bCanTransferContinent);

	//Message
	void SendChatMessage(int nType, const std::string& sMsg);
	void SendNoticeMessage(const std::string& sMsg);
	void SendDropPickUpResultPacket(bool bPickedUp, bool bIsMoney, int nItemID, int nCount, bool bOnExcelRequest);
	void SendDropPickUpFailPacket(bool bOnExcelRequest);
	void SendQuestRecordMessage(int nKey, int nState, const std::string& sStringRecord);
	void SendIncEXPMessage(bool bIsLastHit, int nIncEXP, bool bOnQuest, int nIncEXPBySMQ, int nEventPercentage, int nPartyBonusPercentage, int nPlayTimeHour, int nQuestBonusRate, int nQuestBonusRemainCount, int nPartyBonusEventRate, int nWeddingBonusEXP);
	void SendIncPOPMessage(int nInc);

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
	void OnScriptLinkedQuest(InPacket *iPacket, int nQuestID, int dwTemplateID, Npc *pNpc, int nScriptActCategory);
	void TryQuestStartAct(int nQuestID, int nNpcID, Npc *pNpc);
	void TryQuestCompleteAct(int nQuestID, Npc *pNpc);
	bool TryExchange(const std::vector<ActItem*>& aActItem);
	bool AllowToGetQuestItem(const ActItem* pActionItem);
	void SendQuestResult(int nResult, int nQuestID, int dwTemplateID);

	//Effect
	void SendQuestEndEffect();
	void SendUseSkillEffect(int nSkillID, int nSLV);
	void SendUseSkillEffectByParty(int nSkillID, int nSLV);
	void SendLevelUpEffect();
	void SendChangeJobEffect();
	void SendPlayPortalSE();
	void ShowConsumeItemEffect(int nUserID, bool bShow, int nItemID);

	//Func Key Mapped
	void SendFuncKeyMapped();
	void OnFuncKeyMappedModified(InPacket *iPacket);

	//Pet
	void OnPetPacket(InPacket *iPacket);
	void ActivatePet(int nPos, int nRemoveReaseon, bool bOnInitialize);
	int GetMaxPetIndex();
	void OnActivatePetRequest(InPacket *iPacket);
	void ReregisterPet();
	void UpdateActivePet(unsigned int tCur);
	void OnPetFoodItemUseRequest(InPacket *iPacket);

	//Summoned
	void OnSummonedPacket(InPacket *iPacket);
	void ReregisterSummoned();
	void CreateSummoned(const SkillEntry* pSkill, int nSLV, const FieldPoint& pt, bool bMigrate);
	void RemoveSummoned(int nSkillID, int nLeaveType, int nForceRemoveSkillID); //nForceRemoveSkillID = -1 means that remove all summoneds.
	void RemoveSummoned(Summoned *pSummoned);

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
	void OnOpenEntrustedShop(InPacket *iPacket);

	//Trunk
	void OnTrunkResult(InPacket *iPacket);
	void SetTrunk(Trunk *pTrunk);

	//StoreBank
	ZUniquePtr<StoreBank>& GetStoreBank();
	void SetStoreBank(StoreBank *pStoreBank);

	//Cash Sys
	void OnCenterCashItemResult(InPacket *iPacket);
	void OnCenterCashItemExpireResult(InPacket *iPacket);
	void OnCenterLoadWishListResult(InPacket *iPacket);
	void OnCenterMemoResult(InPacket *iPacket);
	void OnCenterWishListResult(InPacket *iPacket);
	void OnMemoRequest(InPacket *iPacket);

	//TownPortal
	int GetTownPortalFieldID() const;
	void SetTownPortalFieldID(int nTownPortalFieldID);
	const FieldPoint& GetFieldPortalPos() const;
	void SetFieldPortalPos(const FieldPoint& pt);
	void OnCreateTownPortal(int nTownID);
	void OnEnterTownPortalRequest(InPacket *iPacket);
};

