#include "WvsUnified.h"
#include "MemoDBAccessor.h"
#include "GW_Memo.h"
#include "GW_GiftList.h"

#include "..\WvsCenter\CenterPacketTypes.hpp"
#include "..\WvsLib\Net\SocketBase.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Memory\ZMemory.h"

#include <vector>

bool MemoDBAccessor::PostSendMemoRequest(SocketBase * pSrv, int nCharacterID, void * iPacket_)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT COUNT(SN) FROM Memo WHERE CharacterID = " << nCharacterID << " AND State = 1";
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	int nInBox = recordSet["COUNT(SN)"];
	if (nInBox >= GW_Memo::MAX_INBOX_COUNT)
		return false;

	auto iPacket = (InPacket*)iPacket_;
	GW_Memo memo;
	
	memo.nCharacterID = nCharacterID;
	memo.sSender = iPacket->DecodeStr();
	memo.sContent = iPacket->DecodeStr();
	memo.liDateSent = iPacket->Decode8();
	memo.nFlag = iPacket->Decode1();
	memo.nState = 1;

	memo.Save();
	return true;
}

void MemoDBAccessor::PostLoadMemoRequest(SocketBase * pSrv, int nCharacterID)
{
	std::vector <ZUniquePtr<GW_Memo>> aMemo = GW_Memo::Load(nCharacterID);
	if (aMemo.size() > 0)
	{
		OutPacket oPacket;
		oPacket.Encode2(CenterResultPacketType::MemoResult);
		oPacket.Encode4(nCharacterID);
		oPacket.Encode1(GW_Memo::MemoResultType::eMemoRes_Load);
		oPacket.Encode1((int)aMemo.size());
		for (auto& pMemo : aMemo)
			pMemo->Encode(&oPacket);

		pSrv->SendPacket(&oPacket);
	}
}

void MemoDBAccessor::PostLoadGiftListRequest(SocketBase * pSrv, int nCharacterID)
{
	std::vector<ZUniquePtr<GW_GiftList>> aList = GW_GiftList::Load(nCharacterID);

	OutPacket oPacket;
	oPacket.Encode2(CenterResultPacketType::MemoResult);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode1(GW_Memo::MemoResultType::eMemoRes_Load);
	oPacket.Encode2((int)aList.size());
	for (auto& pList : aList)
		pList->Encode(&oPacket);

	pSrv->SendPacket(&oPacket);
}

void MemoDBAccessor::PostDeleteMemoRequest(SocketBase * pSrv, int nCharacterID, void *iPacket_)
{
	auto iPacket = (InPacket*)iPacket_;
	int nCount = iPacket->Decode1();

	GW_Memo memo;
	memo.nCharacterID = nCharacterID;
	memo.nState = 2;

	for (int i = 0; i < nCount; ++i)
	{
		memo.liSN = iPacket->Decode4();
		iPacket->Decode1();
		memo.Save();
	}
}