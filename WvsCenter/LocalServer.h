#pragma once
#include <map>
#include "..\WvsLib\Net\SocketBase.h"

//Server ºÝ¤§ Session
class LocalServer : public SocketBase
{
private:
	std::set<int> m_sUser;
	std::mutex m_mtxUserLock;

	void OnClosed();
	void RemoveConnectedUser(int nUserID);

public:
	LocalServer(asio::io_service& serverService);
	~LocalServer();

	void InsertConnectedUser(int nUserID);
	void OnPacket(InPacket *iPacket);
	void ProcessPacket(InPacket *iPacket);
	void OnRegisterCenterRequest(InPacket *iPacket);
	void OnRequestCharacterList(InPacket *iPacket);
	void OnRequestCreateNewCharacter(InPacket *iPacket);
	void OnRequestGameServerInfo(InPacket *iPacket);
	void OnRequestLoginAuth(InPacket *iPacket);
	void OnRequestMigrateIn(InPacket *iPacket);
	void OnRequestMigrateOut(InPacket *iPacket);
	void OnRequestTransferChannel(InPacket *iPacket);
	void OnRequestMigrateCashShop(InPacket *iPacket);
	void OnGameClientDisconnected(InPacket *iPacket);
	void OnCheckMigrationStateAck(InPacket *iPacket);
	
	//Cash Shop
	void OnRequestBuyCashItem(InPacket *iPacket);
	void OnRequestLoadLocker(InPacket *iPacket);
	void OnReuqestUpdateCash(InPacket *iPacket);
	void OnReuqestMoveLockerToSlot(InPacket *iPacket);
	void OnReuqestMoveSlotToLocker(InPacket *iPacket);

	//World System
	void OnPartyRequest(InPacket *iPacket);
	void OnGuildRequest(InPacket *iPacket);
	void OnGuildBBSRequest(InPacket *iPacket);
	void OnFriendRequest(InPacket *iPacket);
	void OnGroupMessage(InPacket *iPacket);
	void OnWhisperMessage(InPacket *iPacket);
	void OnTrunkRequest(InPacket *iPacket);
	void OnEntrustedShopRequest(InPacket *iPacket);
};

