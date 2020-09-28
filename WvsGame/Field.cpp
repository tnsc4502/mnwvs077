#include "Field.h"
#include "LifePool.h"
#include "..\WvsGame\UserPacketTypes.hpp"
#include "..\WvsGame\ReactorPacketTypes.hpp"
#include "..\WvsGame\MobPacketTypes.hpp"
#include "..\WvsGame\NpcPacketTypes.hpp"
#include "..\WvsGame\FieldPacketTypes.hpp"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "Mob.h"
#include "MovePath.h"
#include "PortalMap.h"
#include "TownPortalPool.h"
#include "ReactorPool.h"
#include "SummonedPool.h"
#include "DropPool.h"
#include "FieldSet.h"
#include "User.h"
#include "PartyMan.h"
#include "WvsPhysicalSpace2D.h"
#include "ContinentMan.h"
#include "AffectedArea.h"
#include "AffectedAreaPool.h"
#include "QWUser.h"
#include "SecondaryStat.h"
#include "..\WvsLib\Memory\ZMemory.h"

#undef min
#undef max

Field::Field(void *pData, int nFieldID)
	: m_pLifePool(AllocObj(LifePool)),
	  m_pPortalMap(AllocObj(PortalMap)),
	  m_pTownPortalPool(AllocObj(TownPortalPool)),
	  m_pReactorPool(AllocObj(ReactorPool)),
	  m_pSpace2D(AllocObj(WvsPhysicalSpace2D))
{
	m_nFieldID = nFieldID;
	m_pDropPool = AllocObjCtor(DropPool)(this);
	m_pSummonedPool = AllocObjCtor(SummonedPool)(this);
	m_pAffectedAreaPool = AllocObjCtor(AffectedAreaPool)(this);

	auto& mapWz = *((WzIterator*)pData);
	auto& infoData = mapWz["info"];
	auto& areaData = mapWz["area"];
	SetFieldID(nFieldID);
	SetCould(((int)infoData["cloud"] != 0));
	SetTown(((int)infoData["town"] != 0));
	SetSwim(((int)infoData["swim"] != 0));
	SetFly(((int)infoData["fly"] != 0));
	SetReturnMap(infoData["returnMap"]);
	SetForcedReturn(infoData["forcedReturn"]);
	SetMobRate(infoData["mobRate"]);
	SetFieldType(infoData["fieldType"]);
	SetFieldLimit(infoData["fieldLimit"]);
	SetCreateMobInterval(infoData["createMobInterval"]);
	SetFiexdMobCapacity(infoData["fixedMobCapacity"]);
	SetFirstUserEnter(infoData["onFirstUerEnter"]);
	SetUserEnter(infoData["onUserEnter"]);
	SetRecoveryRate(std::max(1.0, (double)infoData["recovery"]));
	m_nAutoDecHP = infoData["decHP"];
	m_nAutoDecMP = infoData["decMP"];
	m_nProtectItem = infoData["protectItem"];

	if (areaData != mapWz.end())
		LoadAreaRect(&areaData);

	GetPortalMap()->RestorePortal(this, &(mapWz["portal"]));
	GetReactorPool()->Init(this, &(mapWz["reactor"]));

	auto& seatNode = mapWz["seat"];
	for (auto& seat : seatNode)
		m_aSeat.push_back({ { seat["x"], seat["y"] }, false });
}

Field::~Field()
{
	FreeObj(m_pLifePool);
	FreeObj(m_pPortalMap);
	FreeObj(m_pDropPool);
	FreeObj(m_pSummonedPool);
	FreeObj(m_pSpace2D);
	FreeObj(m_pReactorPool);
	FreeObj(m_pTownPortalPool);
	FreeObj(m_pAffectedAreaPool);
	//m_asyncUpdateTimer->Abort();
	//delete m_asyncUpdateTimer;
}

void Field::BroadcastPacket(OutPacket * oPacket)
{
	std::lock_guard<std::recursive_mutex> userGuard(m_mtxFieldLock);
	oPacket->GetSharedPacket()->ToggleBroadcasting();

	if (m_mUser.size() == 0)
		return;
	for (auto& user : m_mUser)
		user.second->SendPacket(oPacket);
}

void Field::BroadcastPacket(OutPacket* oPacket, std::vector<int>& anCharacterID)
{
	std::lock_guard<std::recursive_mutex> userGuard(m_mtxFieldLock);
	oPacket->GetSharedPacket()->ToggleBroadcasting();
	for (auto& nID : anCharacterID)
	{
		auto iter = m_mUser.find(nID);
		if (iter == m_mUser.end())
			continue;
		
		iter->second->SendPacket(oPacket);
	}
}

void Field::RegisterFieldObj(FieldObj *pNew, OutPacket *oPacketEnter)
{
	std::lock_guard<std::recursive_mutex> userGuard(m_mtxFieldLock);
	oPacketEnter->GetSharedPacket()->ToggleBroadcasting();

	for (auto& user : m_mUser)
		if(pNew->IsShowTo(user.second))
			user.second->SendPacket(oPacketEnter);
}

void Field::SetCould(bool cloud)
{
	m_bCloud = cloud;
}

bool Field::IsCloud() const
{
	return m_bCloud;
}

void Field::SetTown(bool town)
{
	m_bTown = town;
}

bool Field::IsTown() const
{
	return m_bTown;
}

void Field::SetSwim(bool swim)
{
	m_bSwim = swim;
}

bool Field::IsSwim() const
{
	return m_bSwim;
}

void Field::SetFly(bool fly)
{
	m_bFly = fly;
}

bool Field::IsFly() const
{
	return m_bFly;
}

void Field::SetFieldID(int nFieldID)
{
	this->m_nFieldID = nFieldID;
}

int Field::GetFieldID() const
{
	return m_nFieldID;
}

void Field::SetReturnMap(int returnMap)
{
	m_nReturnMap = returnMap;
}

int Field::GetReturnMap() const
{
	return m_nReturnMap;
}

void Field::SetForcedReturn(int forcedReturn)
{
	m_nForcedReturn = forcedReturn;
}

int Field::GetForcedReturn() const
{
	return m_nForcedReturn;
}

void Field::SetMobRate(double dMobRate)
{
	m_dMobRate = dMobRate;
}

double Field::GetMobRate() const
{
	return m_dMobRate;
}

void Field::SetFieldType(int fieldType)
{
	m_nFieldType = fieldType;
}

int Field::GetFieldType() const
{
	return m_nFieldType;
}

void Field::SetFieldLimit(int fieldLimit)
{
	m_nFieldLimit = fieldLimit;
}

int Field::GetFieldLimit() const
{
	return m_nFieldLimit;
}

void Field::SetCreateMobInterval(int interval)
{
	m_nCreateMobInterval = interval;
}

int Field::GetCreateMobInterval() const
{
	return m_nCreateMobInterval;
}

void Field::SetFiexdMobCapacity(int capacity)
{
	m_nFixedMobCapacity = capacity;
}

int Field::GetFixedMobCapacity() const
{
	return m_nFixedMobCapacity;
}

void Field::SetFirstUserEnter(const std::string & script)
{
	m_strFirstUserEnter = script;
}

const std::string & Field::GetFirstUserEnter() const
{
	return m_strFirstUserEnter;
}

void Field::SetUserEnter(const std::string & script)
{
	m_strUserEnter = script;
}

void Field::SetMapSize(int x, int y)
{
	m_szMap.x = x;
	m_szMap.y = y;
}

const FieldPoint & Field::GetMapSize() const
{
	return m_szMap;
}

void Field::SetLeftTop(int x, int y)
{
	m_ptLeftTop.x = x;
	m_ptLeftTop.y = y;
}

const FieldPoint & Field::GetLeftTop() const
{
	return m_ptLeftTop;
}

double Field::GetIncEXPRate() const
{
	return m_dIncRate_EXP;
}

void Field::SetRecoveryRate(double dRate)
{
	m_dRecoveryRate = dRate;
}

double Field::GetRecoveryRate() const
{
	return m_dRecoveryRate;
}

void Field::SetFieldSet(FieldSet * pFieldSet)
{
	m_pParentFieldSet = pFieldSet;
}

FieldSet * Field::GetFieldSet()
{
	return m_pParentFieldSet;
}

void Field::InitLifePool()
{
	std::lock_guard<std::recursive_mutex> lifePoolGuard(m_mtxFieldLock);
	m_pLifePool->Init(this, m_nFieldID);
}

LifePool * Field::GetLifePool()
{
	return m_pLifePool;
}

DropPool * Field::GetDropPool()
{
	return m_pDropPool;
}

void Field::OnEnter(User *pUser)
{
	std::lock_guard<std::recursive_mutex> userGuard(m_mtxFieldLock);
	//if (!m_asyncUpdateTimer->IsStarted())
	//	m_asyncUpdateTimer->Start();
	m_mUser.insert({ pUser->GetUserID(), pUser });
	m_pLifePool->OnEnter(pUser);
	m_pDropPool->OnEnter(pUser);
	m_pReactorPool->OnEnter(pUser);
	if (m_pParentFieldSet != nullptr)
		m_pParentFieldSet->OnUserEnterField(pUser, this);


	OutPacket oPacketForBroadcasting;
	pUser->MakeEnterFieldPacket(&oPacketForBroadcasting);
	RegisterFieldObj(pUser, &oPacketForBroadcasting);

	OutPacket oPacketToTarget;
	for (auto pFieldUser : m_mUser) 
	{
		if (pFieldUser.second->IsShowTo(pUser))
		{
			oPacketToTarget.Reset();
			pFieldUser.second->MakeEnterFieldPacket(&oPacketToTarget);
			pUser->SendPacket(&oPacketToTarget);
		}
	}
	PartyMan::GetInstance()->NotifyTransferField(pUser->GetUserID(), GetFieldID());
	m_pSummonedPool->OnEnter(pUser);
}

void Field::OnLeave(User *pUser)
{
	std::lock_guard<std::recursive_mutex> userGuard(m_mtxFieldLock);
	m_mUser.erase(pUser->GetUserID());
	m_pLifePool->RemoveController(pUser);

	OutPacket oPacketForBroadcasting;
	pUser->MakeLeaveFieldPacket(&oPacketForBroadcasting);
	SplitSendPacket(&oPacketForBroadcasting, nullptr);
}

//Send oPacket to all users in this field, designated user "pExcept" would be ignored if not nullptr.
void Field::SplitSendPacket(OutPacket *oPacket, User *pExcept)
{
	std::lock_guard<std::recursive_mutex> userGuard(m_mtxFieldLock);
	oPacket->GetSharedPacket()->ToggleBroadcasting();

	for (auto& user : m_mUser)
	{
		if ((pExcept == nullptr) || user.second != pExcept)
			user.second->SendPacket(oPacket);
	}
}

void Field::OnPacket(User* pUser, InPacket *iPacket)
{
	int nType = iPacket->Decode2();
	if (nType >= FlagMin(MobRecvPacketType) && nType <= 0x9A)
		m_pLifePool->OnPacket(pUser, nType, iPacket);
	else if (nType == UserRecvPacketType::User_OnUserPickupRequest)
		m_pDropPool->OnPacket(pUser, nType, iPacket);
	else if (nType == FieldRecvPacketType::Field_OnContiMoveStateRequest)
		OnContiMoveState(pUser, iPacket);
	else if (nType >= FlagMin(ReactorRecvPacketType) && nType <= FlagMax(ReactorRecvPacketType))
		m_pReactorPool->OnPacket(pUser, nType, iPacket);
}

void Field::OnUserMove(User * pUser, InPacket * iPacket)
{
	iPacket->Decode1();
	MovePath movePath;
	movePath.Decode(iPacket);
	auto& lastElem = movePath.m_lElem.rbegin();
	pUser->SetMovePosition(lastElem->x, lastElem->y, lastElem->bMoveAction, lastElem->fh);
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserRemote_OnMove);
	oPacket.Encode4(pUser->GetUserID());
	movePath.Encode(&oPacket);
	this->SplitSendPacket(&oPacket, pUser);
	//GetLifePool()->UpdateMobSplit(pUser);
}

PortalMap * Field::GetPortalMap()
{
	return m_pPortalMap;
}

TownPortalPool * Field::GetTownPortalPool()
{
	return m_pTownPortalPool;
}

ReactorPool * Field::GetReactorPool()
{
	return m_pReactorPool;
}

SummonedPool * Field::GetSummonedPool()
{
	return m_pSummonedPool;
}

std::recursive_mutex & Field::GetFieldLock()
{
	return m_mtxFieldLock;
}

WvsPhysicalSpace2D * Field::GetSpace2D()
{
	return m_pSpace2D;
}

AffectedAreaPool *Field::GetAffectedAreaPool()
{
	return m_pAffectedAreaPool;
}

const std::map<int, User*>& Field::GetUsers()
{
	return m_mUser;
}

void Field::OnMobMove(User * pCtrl, Mob * pMob, InPacket * iPacket)
{
	//_ZtlSecureTear_m_nMobCtrlSN
	short nMobCtrlSN = iPacket->Decode2();

	char flag = iPacket->Decode1();
	char bNextAttackPossible = (flag & 0x0F) != 0;
	char pCenterSplit = iPacket->Decode1();

	//ZtlSecureTear_bSN_CS
	unsigned char nSkillCommand = 0;
	unsigned char nSLV = 0;
	short nSkillEffect = 0;
	int nData = iPacket->Decode4();

	bool bShootAttack = false;
	pMob->OnMobMove(
		bNextAttackPossible == 1,
		(pCenterSplit < 0 ? -1 : pCenterSplit >> 1),
		nData,
		&nSkillCommand,
		&nSLV,
		&bShootAttack);

	iPacket->Decode1();
	iPacket->Decode4();

	MovePath movePath;
	movePath.Decode(iPacket);

	//Encode Ctrl Ack Packet
	OutPacket ctrlAckPacket;
	ctrlAckPacket.Encode2(MobSendPacketType::Mob_OnCtrlAck);
	ctrlAckPacket.Encode4(pMob->GetFieldObjectID());
	ctrlAckPacket.Encode2(nMobCtrlSN);
	ctrlAckPacket.Encode1(bNextAttackPossible);
	ctrlAckPacket.Encode2((int)pMob->GetMP());
	ctrlAckPacket.Encode1(nSkillCommand);
	ctrlAckPacket.Encode1(nSLV);

	//Encode Move Packet
	OutPacket movePacket;
	movePacket.Encode2(MobSendPacketType::Mob_OnMove); //CMob::OnMove
	movePacket.Encode4(pMob->GetFieldObjectID());
	movePacket.Encode1(bNextAttackPossible);
	movePacket.Encode1(pCenterSplit);

	//dwData
	movePacket.Encode4(nData);

	std::lock_guard<std::recursive_mutex> lifeGuard(m_pLifePool->GetLock());
	auto& elem = *(movePath.m_lElem.rbegin());
	{
		elem.y += -1;
		auto& elem = *(movePath.m_lElem.rbegin());
		pMob->SetPosX(elem.x);
		pMob->SetPosY(elem.y);
		pMob->SetMoveAction(elem.bMoveAction);
		pMob->SetFh(elem.fh);
	}
	movePath.Encode(&movePacket);
	pCtrl->SendPacket(&ctrlAckPacket);
	SplitSendPacket(&movePacket, pCtrl);
}

void Field::LoadAreaRect(void *pData)
{
	auto& areaData = *((WzIterator*)pData);
	FieldRect rect;
	for (auto& area : areaData)
	{
		rect.left = area["x1"];
		rect.top = area["y1"];
		rect.right = area["x2"];
		rect.bottom = area["y2"];
		m_mAreaRect[area.GetName()] = rect;
	}
}

int Field::CountFemaleInArea(const std::string & sArea)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldLock);
	auto findIter = m_mAreaRect.find(sArea);
	if (findIter == m_mAreaRect.end())
		return 0;
	int nRet = 0;
	for (auto pUser : m_mUser)
	{
		if (!pUser.second->GetCharacterData()->mStat->nGender)
			continue;
		if (findIter->second.PtInRect({ pUser.second->GetPosX(), pUser.second->GetPosY() }))
			++nRet;
	}
	return nRet;
}

int Field::CountMaleInArea(const std::string & sArea)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldLock);
	auto findIter = m_mAreaRect.find(sArea);
	if (findIter == m_mAreaRect.end())
		return 0;
	int nRet = 0;
	for (auto pUser : m_mUser)
	{
		if (pUser.second->GetCharacterData()->mStat->nGender)
			continue;
		if (findIter->second.PtInRect({ pUser.second->GetPosX(), pUser.second->GetPosY() }))
			++nRet;
	}
	return nRet;
}

int Field::CountUserInArea(const std::string & sArea)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldLock);
	auto findIter = m_mAreaRect.find(sArea);
	if (findIter == m_mAreaRect.end())
		return 0;
	int nRet = 0;
	for (auto pUser : m_mUser)
	{
		if (findIter->second.PtInRect({ pUser.second->GetPosX(), pUser.second->GetPosY() }))
			++nRet;
	}
	return nRet;
}

void Field::EffectScreen(const std::string& sEffect)
{
	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketType::Field_OnFieldEffect);
	oPacket.Encode1(FieldEffect::e_FieldEffect_Screen);
	oPacket.EncodeStr(sEffect);
	BroadcastPacket(&oPacket);
}

void Field::EffectScreen(const std::string& sEffect, std::vector<int>& anCharacterID)
{
	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketType::Field_OnFieldEffect);
	oPacket.Encode1(FieldEffect::e_FieldEffect_Screen);
	oPacket.EncodeStr(sEffect);
	BroadcastPacket(&oPacket, anCharacterID);
}

void Field::EffectSound(const std::string& sEffect)
{
	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketType::Field_OnFieldEffect);
	oPacket.Encode1(FieldEffect::e_FieldEffect_Sound);
	oPacket.EncodeStr(sEffect);
	BroadcastPacket(&oPacket);
}

void Field::EffectSound(const std::string& sEffect, std::vector<int>& anCharacterID)
{
	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketType::Field_OnFieldEffect);
	oPacket.Encode1(FieldEffect::e_FieldEffect_Sound);
	oPacket.EncodeStr(sEffect);
	BroadcastPacket(&oPacket, anCharacterID);
}

void Field::EffectObject(const std::string& sEffect)
{
	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketType::Field_OnFieldEffect);
	oPacket.Encode1(FieldEffect::e_FieldEffect_Object);
	oPacket.EncodeStr(sEffect);
	BroadcastPacket(&oPacket);
}

void Field::EnablePortal(const std::string& sPortal, bool bEnable)
{
	m_pPortalMap->EnablePortal(sPortal, bEnable);
}

void Field::OnContiMoveState(User * pUser, InPacket * iPacket)
{
	int nFieldID = iPacket->Decode4();
	int nEventDoing = ContinentMan::GetInstance()->GetInfo(nFieldID, 0);
	int nState = ContinentMan::GetInstance()->GetInfo(nFieldID, 1);
	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketType::Field_OnContiState);
	oPacket.Encode1(nState);
	oPacket.Encode1((char)nEventDoing);
	pUser->SendPacket(&oPacket);
}

bool Field::OnSitRequest(User *pUser, int nSeatID)
{
	if(nSeatID >= (int)m_aSeat.size())
		return false;

	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldLock);
	if (nSeatID >= 0 &&
		m_aSeat[nSeatID].second == false)
	{
		if (m_aSeat[nSeatID].first.Range(FieldPoint({ pUser->GetPosX(), pUser->GetPosY() })) >= 200)
			return false;

		m_aSeat[nSeatID].second = true;
		m_mUserSeat[pUser->GetUserID()] = nSeatID;
		return true;
	}
	else
	{
		auto findIter = m_mUserSeat.find(pUser->GetUserID());
		if (findIter == m_mUserSeat.end())
			return false;
		m_aSeat[findIter->second].second = false;
		m_mUserSeat.erase(findIter);
		return true;
	}
	return false;
}

void Field::AddCP(int nLastDamageCharacterID, int nAddCP)
{
}

void Field::TransferAll(int nFieldID, const std::string& sPortal)
{
	std::lock_guard<std::recursive_mutex> userGuard(m_mtxFieldLock);
	auto iter = m_mUser.begin();
	while (iter != m_mUser.end())
		(iter++)->second->TryTransferField(nFieldID, sPortal);
}

void Field::OnReactorDestroyed(Reactor* pReactor)
{
}

void Field::CheckReactorAction(const std::string &sReactorName, unsigned tEventTime)
{
	if (m_pParentFieldSet)
		m_pParentFieldSet->CheckReactorAction(this, sReactorName, tEventTime);
}

void Field::Reset(bool bShuffleReactor)
{
	m_pDropPool->TryExpire(true);
	m_pLifePool->Reset();
	m_pReactorPool->Reset(false);
	m_pPortalMap->ResetPortal();
}

void Field::OnStatChangeByField(unsigned int tCur)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldLock);
	long long int liFlag = 0;
	if (tCur - m_tLastStatChangeByField > FIELD_STAT_CHANGE_PERIOD &&
		(m_nAutoDecHP || m_nAutoDecMP))
	{
		for (auto& prUser : m_mUser)
		{
			if ((prUser.second->GetGradeCode() & User::UserGrade::eGrade_GM) ||
				prUser.second->GetCharacterData()->IsWearing(m_nProtectItem) ||
				prUser.second->GetSecondaryStat()->nThaw_)
				continue;

			liFlag |= QWUser::IncHP(prUser.second, -m_nAutoDecHP, false);
			if (prUser.second->GetCharacterData()->mStat->nHP == 0)
				prUser.second->OnUserDead(IsTown());
			liFlag |= QWUser::IncMP(prUser.second, -m_nAutoDecMP, false);
			prUser.second->SendCharacterStat(false, liFlag);
		}
		m_tLastStatChangeByField = tCur;
	}
}

void Field::Update()
{
	unsigned int tCur = GameDateTime::GetTime();
	m_pLifePool->Update();
	m_pReactorPool->Update(tCur); 
	m_pAffectedAreaPool->Update(tCur);
	m_pDropPool->TryExpire(false);
	m_pSummonedPool->Update(tCur);
	OnStatChangeByField(tCur);
}
