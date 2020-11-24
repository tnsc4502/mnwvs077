#pragma once
#include "..\WvsLib\Memory\ZMemory.h"
#include <vector>
#include <string>

class InPacket;
class OutPacket;

struct GW_Memo
{
	const static int MAX_INBOX_COUNT = 30;

	enum MemoType
	{
		eMemo_Default = 0,
		eMemo_IncPOP,
		eMemo_Invitation,
		eMemo_BrokeUp,
		eMemo_Divorced
	};

	enum MemoRequestType
	{
		eMemoReq_Send = 0,
		eMemoReq_Load,
		eMemoReq_Delete,
	};

	enum MemoResultType
	{
		eMemoRes_Load = 2,
		eMemoRes_SendSuccess,
		eMemoRes_SendFailed,
	};

	enum MemoSendFailReason
	{
		eMemoFailReason_UserIsOnline = 0,
		eMemoFailReason_UserDoesNotExist,
		eMemoFailReason_UserInBoxIsFull,
	};

	long long int liDateSent, liSN = -1;
	int nCharacterID, nFlag, nState;
	std::string sSender, sContent;

#if defined(DBLIB) || defined(_WVSCENTER)
	void Load(void *pRecordSet);
	void Save();
#endif
	void Encode(OutPacket *oPacket);
	void Decode(InPacket *iPacket);

	static std::vector<ZUniquePtr<GW_Memo>> Load(int nCharacterID);
};

