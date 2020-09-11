#pragma once
#include <set>
#include <mutex>
#include "asio.hpp"

class OutPacket;
class InPacket;

class SocketBase : public std::enable_shared_from_this<SocketBase>
{
public:
	enum SocketStatus
	{
		eClosed = 0x01,
		eConnecting = 0x02,
		eConnected = eConnecting | 0x04
	};

private:

	static std::mutex stSocketRecordMtx;
	static std::set<unsigned int> stSocketIDRecord;
	unsigned char m_nServerType;
	unsigned int m_nSocketID;

	static unsigned int DesignateSocketID();
	static void ReleaseSocketID(unsigned int nSocketID);

	asio::ip::tcp::socket m_Socket;
	asio::ip::tcp::resolver m_Resolver;
	std::mutex m_mtxLock;

	unsigned char* m_aRecvIV, *m_aSendIV;
	SocketStatus m_eSocketStatus = SocketStatus::eClosed;

	//Note : this flag indicates the role of this socket (true = local server).
	//You should note that the local server won't encrypt and decrypt packets, and won't send game server info.
	bool m_bIsLocalServer = false;

	void EncodeHandShakeInfo(OutPacket *oPacket);

	//Decrease reference counter of given shared packet. 
	void OnSendPacketFinished(const std::error_code &ec, std::size_t bytes_transferred, unsigned char *buffer, void *pPacket);
	void OnReceive(const std::error_code &ec, std::size_t bytes_transferred, unsigned char* buffer);
	void ProcessPacket(const std::error_code &ec, std::size_t bytes_transferred, unsigned char* buffer, int nPacketLen);
	//void(*OnNotifySocketDisconnected)(SocketBase *pSocket);
	std::function<void(SocketBase *)> m_fSocketDisconnectedCallBack;

#if defined(_WVSSHOP) || defined(_WVSGAME) || defined(_WVSLOGIN) || defined(_WVSLIB)
	void OnResolve(const std::error_code& err, asio::ip::tcp::resolver::iterator endpoint_iterator);
	void OnConnectResult(const std::error_code& err, asio::ip::tcp::resolver::iterator endpoint_iterator);
#endif

protected:
	void OnWaitingPacket();
	virtual void OnClosed() = 0;

public:
	SocketBase(asio::io_service& serverService, bool bIsLocalServer = false);
	virtual ~SocketBase();

	asio::ip::tcp::socket& GetSocket();
	SocketStatus GetSocketStatus() const;
	bool CheckSocketStatus(SocketStatus e) const;
	unsigned int GetSocketID() const;
	unsigned char GetServerType();
	void SetServerType(unsigned char nType);
	void SetSocketDisconnectedCallBack(const std::function<void(SocketBase *)>& fObject);

	void Init();
	void SendPacket(OutPacket *iPacket, bool bIsHandShakePacket = false);
	void OnDisconnect();
	virtual void OnPacket(InPacket *iPacket) = 0;

#if defined(_WVSSHOP) || defined(_WVSGAME) || defined(_WVSLOGIN) || defined(_WVSLIB)
	void Connect(const std::string& strAddr, short nPort);
	virtual void OnConnected();
	virtual void OnConnectFailed();
#endif
};
