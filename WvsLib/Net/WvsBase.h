#pragma once
#include <map>
#include "asio.hpp"
#include "SocketBase.h"
#include <functional>
#include "..\Logger\WvsLogger.h"

class WvsBase
{
private:
	asio::io_service m_IOService;
	asio::ip::tcp::acceptor *m_pAcceptor;
	static std::map<unsigned int, SocketBase*> m_mSocketList;
	int m_nExternalPort = 0, m_aExternalIP[4];

	template<typename SOCKET_TYPE>
	void OnAccepted(std::shared_ptr<SOCKET_TYPE> pAcceptedSocket, const std::error_code& ec)
	{
		WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "[WvsBase]A new connection is established.\n");
		OnSocketConnected((SocketBase*)(pAcceptedSocket.get()));
		
		pAcceptedSocket->SetSocketDisconnectedCallBack(std::bind(&WvsBase::OnSocketDisconnected, this, std::placeholders::_1));
		pAcceptedSocket->Init();
		BeginAccept<SOCKET_TYPE>();
	}

protected:
	WvsBase();
	virtual ~WvsBase();

public:

	//Actually I just don't want to define GetInstance in every wvs based class.
	template<typename WVS_TYPE>
	static WVS_TYPE *GetInstance()
	{
		static WVS_TYPE *pInstance = new WVS_TYPE();
		return pInstance;
	}

	void OnSocketConnected(SocketBase *pSocket);
	void OnSocketDisconnected(SocketBase *pSocket);
	virtual void OnNotifySocketDisconnected(SocketBase *pSocket) = 0;

	void CreateAcceptor(short nPort);

	//The template argument "SOCKET_TYPE" denotes the client implementation class.
	//Here you should also notice that the declaration and definition of template function should not be separated.
	template<typename SOCKET_TYPE>
	void BeginAccept()
	{
		std::shared_ptr<SOCKET_TYPE> session(new SOCKET_TYPE(GetIOService()));
		m_pAcceptor->async_accept(session->GetSocket(), std::bind(&WvsBase::OnAccepted<SOCKET_TYPE>, this, session, std::placeholders::_1));
	}

	virtual void Init();
	asio::io_service& GetIOService();

	void SetExternalIP(const std::string& ip);
	void SetExternalPort(short nPort);
	int* GetExternalIP() const;
	short GetExternalPort() const;

	const std::map<unsigned int, SocketBase*>& GetSocketList() const;
	SocketBase* GetSocket(unsigned int nSocketID);

};

