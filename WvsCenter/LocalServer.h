#pragma once
#include <map>
#include "..\WvsLib\Net\SocketBase.h"

//Server ºÝ¤§ Session
class LocalServer : public SocketBase
{
private:
	std::set<int> m_sUser;
	void OnClosed();

public:
	LocalServer(asio::io_service& serverService);
	~LocalServer();

	void OnPacket(InPacket *iPacket);
	void OnRegisterCenterRequest(InPacket *iPacket);
	void OnRequestCharacterList(InPacket *iPacket);
	void OnRequestCreateNewCharacter(InPacket *iPacket);
	void OnRequestGameServerInfo(InPacket *iPacket);
	void OnRequestMigrateIn(InPacket *iPacket);
	void OnRequestMigrateOut(InPacket *iPacket);
	void OnRequestTransferChannel(InPacket *iPacket);
	void OnRequestMigrateCashShop(InPacket *iPacket);
	
	//Cash Shop
	void OnRequestBuyCashItem(InPacket *iPacket);
	void OnRequestLoadLocker(InPacket *iPacket);
	void OnReuqestUpdateCash(InPacket *iPacket);
	void OnReuqestMoveLockerToSlot(InPacket *iPacket);
	void OnReuqestMoveSlotToLocker(InPacket *iPacket);

	//Party
	void OnPartyRequest(InPacket *iPacket);

	//Guild
	void OnGuildRequest(InPacket *iPacket);
};

