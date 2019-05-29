#include "EntrustedShopMan.h"
#include "LocalServer.h"
#include "WvsWorld.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"

EntrustedShopMan::EntrustedShopMan()
{
}

EntrustedShopMan::~EntrustedShopMan()
{
}

EntrustedShopMan* EntrustedShopMan::GetInstance()
{
	static EntrustedShopMan *pInstance = new EntrustedShopMan;

	return pInstance;
}

void EntrustedShopMan::CheckEntrustedShopOpenPossible(LocalServer* pSrv, int nCharacterID, long long int liCashItemSN)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	auto pwUser = WvsWorld::GetInstance()->GetUser(nCharacterID);
	if (pwUser)
	{
		OutPacket oPacket;
		oPacket.Encode2(CenterSendPacketFlag::RemoteBroadcasting);
		oPacket.Encode4(nCharacterID);
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnEntrustedShopCheckResult);
		if (m_sEmployer.find(nCharacterID) == m_sEmployer.end())
			oPacket.Encode1(EntrustedShopCheckResult::res_EShop_OpenCheck_Valid);
		else
			oPacket.Encode1(EntrustedShopCheckResult::res_EShop_OpenCheck_Failed);
		pwUser->SendPacket(&oPacket);
	}
}

void EntrustedShopMan::CreateEntrustedShop(LocalServer * pSrv, int nCharacterID, int SlotCount, long long int liCashItemSN)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	m_sEmployer.insert(nCharacterID);
}

void EntrustedShopMan::RemoveEntrustedShop(LocalServer * pSrv, int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	m_sEmployer.erase(nCharacterID);
}
