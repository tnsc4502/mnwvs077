#pragma once
#include <map>
#include <mutex>
#include <vector>
#include <functional>
#include "FieldPoint.h"
#include "FieldRect.h"

class LifePool;
class Mob;
class Portal;
class PortalMap;
class TownPortalPool;
class ReactorPool;
class Reactor;
class DropPool;
class FieldSet;
class User;
class AsyncScheduler;
class InPacket;
class OutPacket;
class WvsPhysicalSpace2D;
class SummonedPool;
class AffectedAreaPool;
class FieldObj;

class Field
{
protected:
	static const int FIELD_STAT_CHANGE_PERIOD = 3 * 1000;

	struct BalloonEntry
	{
		enum BalloonType
		{
			e_Balloon_EntrustedShop = 0x02,
		};

		int nBalloonType = 0;
		int nHostX = 0, nHostY = 0;
	};

	enum FieldEffect
	{
		e_FieldEffect_Object = 0x02,
		e_FieldEffect_Screen = 0x03,
		e_FieldEffect_Sound = 0x04
	};

	std::recursive_mutex m_mtxFieldLock;
	std::map<int, User*> m_mUser; //m_lUser in WvsGame.idb
	std::map<std::string, FieldRect> m_mAreaRect;
	std::vector<std::pair<FieldPoint, bool>> m_aSeat;
	std::map<int, int> m_mUserSeat;
	int m_nFieldID = 0;
	LifePool *m_pLifePool;
	PortalMap *m_pPortalMap;
	DropPool *m_pDropPool;
	TownPortalPool *m_pTownPortalPool;
	ReactorPool *m_pReactorPool;
	SummonedPool* m_pSummonedPool;
	FieldSet* m_pParentFieldSet = nullptr;
	WvsPhysicalSpace2D* m_pSpace2D;
	FieldPoint m_ptLeftTop, m_szMap;
	AffectedAreaPool* m_pAffectedAreaPool;

	std::string m_sStreetName, 
				m_sMapName;

	double m_dIncRate_EXP = 1.0, 
		   m_dRecoveryRate = 1.0,
		   m_dIncRate_Drop = 1.0,
			m_dMobRate = 1.0;

	bool m_bCloud,
		m_bTown,
		m_bSwim,
		m_bFly;

	int m_nReturnMap,
		m_nForcedReturn,
		m_nFieldType,
		m_nFieldLimit,
		m_nAutoDecHP = 0,
		m_nAutoDecMP = 0,
		m_nProtectItem = 0,
		m_nCreateMobInterval,
		m_nFixedMobCapacity;

	unsigned int m_tLastStatChangeByField = 0;

	std::string m_strFirstUserEnter, 
				m_strUserEnter;

	//Weather & JukeBox
	int m_nWeatherItemID = 0,
		m_nJukeBoxItemID = 0,
		m_nJBCharacterID = 0;

	unsigned int
		m_tWeatherBegin = 0,
		m_tJukeBoxEnd = 0;

	bool m_bWeatherByAdmin = false;

	std::string 
		m_sWeatherMsg,
		m_sJBCharacterName;

public:
	Field(void *pData, int nFieldID);
	~Field();

	//GET/SET
	void SetCould(bool cloud);
	bool IsCloud() const;
	void SetTown(bool town);
	bool IsTown() const;
	void SetSwim(bool swim);
	bool IsSwim() const;
	void SetFly(bool fly);
	bool IsFly() const;
	void SetFieldID(int nFieldID);
	int GetFieldID() const;
	void SetReturnMap(int returnMap);
	int GetReturnMap() const;
	void SetForcedReturn(int forcedReturn);
	int GetForcedReturn() const;
	void SetMobRate(double dMobRate);
	double GetMobRate() const;
	void SetFieldType(int fieldType);
	int GetFieldType() const;
	void SetFieldLimit(int fieldLimit);
	int GetFieldLimit() const;
	void SetCreateMobInterval(int interval);
	int GetCreateMobInterval() const;
	void SetFiexdMobCapacity(int capacity);
	int GetFixedMobCapacity() const;
	void SetFirstUserEnter(const std::string& script);
	const std::string& GetFirstUserEnter() const;
	void SetUserEnter(const std::string& script);
	const std::string& GetUserEnter() const;
	void SetMapSize(int x, int y);
	const FieldPoint& GetMapSize() const;
	void SetLeftTop(int x, int y);
	const FieldPoint& GetLeftTop() const;
	double GetIncEXPRate() const;
	void SetRecoveryRate(double dRate);
	double GetRecoveryRate() const;

	void SetFieldSet(FieldSet *pFieldSet);
	FieldSet *GetFieldSet();

	//Pools
	void InitLifePool();
	LifePool *GetLifePool();
	DropPool *GetDropPool();
	PortalMap* GetPortalMap();
	TownPortalPool* GetTownPortalPool();
	ReactorPool* GetReactorPool();
	SummonedPool* GetSummonedPool();
	WvsPhysicalSpace2D* GetSpace2D();
	AffectedAreaPool* GetAffectedAreaPool();

	//Field fundamental
	virtual void OnEnter(User *pUser);
	virtual void OnLeave(User *pUser);
	void SplitSendPacket(OutPacket* oPacket, User* pExcept);
	void BroadcastPacket(OutPacket* oPacket);
	void BroadcastPacket(OutPacket* oPacket, std::vector<int>& anCharacterID);
	void RegisterFieldObj(FieldObj *pNew, OutPacket *oPacketEnter);
	std::recursive_mutex& GetFieldLock();

	//Users & Mobs
	virtual void OnPacket(User* pUser, InPacket* iPacket);
	void OnMobMove(User* pCtrl, Mob* pMob, InPacket* iPacket);
	void OnUserMove(User* pUser, InPacket *iPacket);
	const std::map<int, User*>& GetUsers();
	void TransferAll(int nFieldID, const std::string& sPortal);

	//Party Quest Helpers
	void LoadAreaRect(void *pData);
	int CountFemaleInArea(const std::string& sArea);
	int CountMaleInArea(const std::string& sArea);
	int CountUserInArea(const std::string& sArea);

	//Effects
	void EffectScreen(const std::string& sEffect);
	void EffectScreen(const std::string& sEffect, std::vector<int>& anCharacterID);
	void EffectSound(const std::string& sEffect);
	void EffectSound(const std::string& sEffect, std::vector<int>& anCharacterID);
	void EffectObject(const std::string& sEffect);
	void EnablePortal(const std::string& sPortal, bool bEnable);

	//Weather & JukeBox
	void OnWeather(int nItemID, const std::string& sUserName, const std::string& sMsg);
	void OnJukeBox(int nItemID, unsigned int tDuration, User *pUser);
	void EncodeWeather(OutPacket *oPacket);
	void EncodeJukeBox(OutPacket *oPacket);

	//Events
	void OnContiMoveState(User *pUser, InPacket *iPacket);
	bool OnSitRequest(User *pUser, int nSeatID);
	virtual void OnReactorDestroyed(Reactor *pReactor);
	virtual void AddCP(int nLastDamageCharacterID, int nAddCP);
	void CheckReactorAction(const std::string& sReactorName, unsigned tEventTime);
	virtual void Reset(bool bShuffleReactor);
	virtual void OnStatChangeByField(unsigned int tCur);
	virtual void Update();
};

