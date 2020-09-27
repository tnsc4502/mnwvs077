#include "SocketBase.h"
#include "..\Common\ServerConstants.hpp"
#include "OutPacket.h"
#include "InPacket.h"
#include <functional>
#include <iostream>

#include "..\Memory\MemoryPoolMan.hpp"
#include "..\Logger\WvsLogger.h"

#include "..\Crypto\WvsCrypto.hpp"
#include "..\Logger\WvsLogger.h"

std::mutex SocketBase::stSocketRecordMtx;
std::set<unsigned int> SocketBase::stSocketIDRecord;


SocketBase::SocketBase(asio::io_service& serverService, bool bIsLocalServer)
	: m_Socket(serverService),
	m_Resolver(serverService),
	m_bIsLocalServer(bIsLocalServer),
	m_nSocketID(SocketBase::DesignateSocketID()),
	m_aRecvIV((unsigned char*)AllocArray(char, 16)),
	m_aSendIV((unsigned char*)AllocArray(char, 16))
{
}

SocketBase::~SocketBase()
{
	FreeArray(m_aRecvIV);
	FreeArray(m_aSendIV);
}

void SocketBase::SetSocketDisconnectedCallBack(const std::function<void(SocketBase *)>& fObject)
{
	m_fSocketDisconnectedCallBack = fObject;
}

void SocketBase::SetServerType(unsigned char nType)
{
	m_nServerType = nType;
}

unsigned char SocketBase::GetServerType()
{
	return m_nServerType;
}

void SocketBase::Init()
{
	asio::ip::tcp::no_delay option(true);
	m_Socket.set_option(option);

	std::unique_ptr<OutPacket> oPacket{ new OutPacket() };
	EncodeHandShakeInfo(oPacket.get());

	if (!m_bIsLocalServer)
		SendPacket(oPacket.get(), true);

	OnWaitingPacket();
}

void SocketBase::OnDisconnect()
{
	m_eSocketStatus = SocketStatus::eClosed;
	ReleaseSocketID(GetSocketID());
	m_fSocketDisconnectedCallBack(this);
	m_Socket.close();
	OnClosed();
}

void SocketBase::Connect(const std::string & strAddr, short nPort)
{
	m_eSocketStatus = SocketStatus::eConnecting;
	asio::ip::tcp::resolver::query centerSrvQuery(strAddr, std::to_string(nPort));

	m_Resolver.async_resolve(centerSrvQuery,
		std::bind(&SocketBase::OnResolve, std::dynamic_pointer_cast<SocketBase>(shared_from_this()),
			std::placeholders::_1,
			std::placeholders::_2));
}

void SocketBase::OnResolve(const std::error_code & err, asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	if (!err)
	{
		asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
		GetSocket().async_connect(endpoint,
			std::bind(&SocketBase::OnConnectResult, std::static_pointer_cast<SocketBase>(shared_from_this()),
				std::placeholders::_1, ++endpoint_iterator));
	}
	else
	{
		OnConnectFailed();
		return;
	}
}

void SocketBase::OnConnectResult(const std::error_code & err, asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	if (err)
	{
		m_eSocketStatus = SocketStatus::eClosed;
		OnConnectFailed();
	}
	else
	{
		m_eSocketStatus = SocketStatus::eConnected;
		OnConnected();
	}
}

void SocketBase::OnConnected()
{
}

void SocketBase::OnConnectFailed()
{
}

unsigned int SocketBase::DesignateSocketID()
{
	static bool bRandSeedSet = false;
	if (!bRandSeedSet)
	{
		srand((unsigned int)time(nullptr));
		bRandSeedSet = true;
	}
	const unsigned int RESERVED_INDEX = 100;
	const unsigned int MAX_RAND_RANGE = RESERVED_INDEX + (INT_MAX - 1 - RESERVED_INDEX) * 2;
	long long int liRnd = 0;
	unsigned int nRndSocketID = 0;
	std::lock_guard<std::mutex> lock(stSocketRecordMtx);
	do
	{
		liRnd = (long long int)(rand() % 0x7FFF) * (long long int)(rand() % 0x7FFF) * (long long int)(rand() % 0x7FFF) * (long long int)(rand() % 0x7FFF);
		nRndSocketID = RESERVED_INDEX + liRnd % ((long long int)MAX_RAND_RANGE);
	} 
	while ((stSocketIDRecord.find(nRndSocketID) != stSocketIDRecord.end()));
	stSocketIDRecord.insert(nRndSocketID);
	return nRndSocketID;
}

void SocketBase::ReleaseSocketID(unsigned int nSocketID)
{
	std::lock_guard<std::mutex> lock(stSocketRecordMtx);
	stSocketIDRecord.erase(nSocketID);
}

void SocketBase::EncodeHandShakeInfo(OutPacket *oPacket)
{
	oPacket->Encode2(0x0D);
	oPacket->Encode2(ServerConstants::kGameVersion);
	oPacket->EncodeStr(ServerConstants::strGameSubVersion);
	oPacket->Encode4(*(int*)m_aRecvIV);
	oPacket->Encode4(*(int*)m_aSendIV);
	oPacket->Encode1(ServerConstants::kGameLocale);
	//oPacket->Encode1(0x00);
}

asio::ip::tcp::socket& SocketBase::GetSocket()
{
	return m_Socket;
}

SocketBase::SocketStatus SocketBase::GetSocketStatus() const
{
	return m_eSocketStatus;
}

bool SocketBase::CheckSocketStatus(SocketStatus e) const
{
	return (m_eSocketStatus & e) == e;
}

void SocketBase::SendPacket(OutPacket *oPacket, bool bIsHandShakePacket)
{
	std::lock_guard<std::mutex> lock(m_mtxLock);
	if (!m_Socket.is_open())
	{
		if(m_eSocketStatus != SocketStatus::eClosed)
			OnDisconnect();
		return;
	}
	oPacket->IncRefCount();

	auto pBuffer = oPacket->GetPacket();

	/*
	[+] 19/05/07
	Since the encrypt function would directly write on passed-buffer, 
	it is necessary to replicate a buffer for broadcasting.
	*/
	if (oPacket->GetSharedPacket()->IsBroadcasting()) 
	{
		pBuffer = AllocArray(unsigned char, oPacket->GetPacketSize() + OutPacket::HEADER_OFFSET) + OutPacket::HEADER_OFFSET;
		memcpy(pBuffer, oPacket->GetPacket(), oPacket->GetPacketSize());
		oPacket->GetSharedPacket()->AttachBroadcastingPacket(pBuffer - OutPacket::HEADER_OFFSET);
	}

	if (!bIsHandShakePacket)
	{
		WvsCrypto::InitializeEncryption(pBuffer - OutPacket::HEADER_OFFSET, m_aSendIV, oPacket->GetPacketSize());
		if (!m_bIsLocalServer)
			WvsCrypto::Encrypt(pBuffer, m_aSendIV, oPacket->GetPacketSize());
		asio::async_write(m_Socket,
			asio::buffer(pBuffer - OutPacket::HEADER_OFFSET, oPacket->GetPacketSize() + OutPacket::HEADER_OFFSET),
			std::bind(&SocketBase::OnSendPacketFinished,
				shared_from_this(), 
				std::placeholders::_1, 
				std::placeholders::_2, 
				pBuffer - (OutPacket::HEADER_OFFSET),
				oPacket->GetSharedPacket()));
	}
	else
	{
		asio::async_write(m_Socket,
			asio::buffer(pBuffer, oPacket->GetPacketSize()),
			std::bind(&SocketBase::OnSendPacketFinished,
				shared_from_this(), 
				std::placeholders::_1, 
				std::placeholders::_2, 
				pBuffer - (OutPacket::HEADER_OFFSET),
				oPacket->GetSharedPacket()));
	}
}

void SocketBase::OnSendPacketFinished(const std::error_code &ec, std::size_t bytes_transferred, unsigned char *buffer, void *pPacket)
{
	((OutPacket::SharedPacket*)pPacket)->DecRefCount();
}

void SocketBase::OnWaitingPacket()
{
	auto buffer = AllocArray(unsigned char, 4);
	asio::async_read(m_Socket,
		asio::buffer(buffer, 4),
		std::bind(&SocketBase::OnReceive,
			shared_from_this(), std::placeholders::_1, std::placeholders::_2, buffer));
}

void SocketBase::OnReceive(const std::error_code &ec, std::size_t bytes_transferred, unsigned char* buffer)
{
	if (!ec)
	{
		unsigned short nPacketLen = WvsCrypto::GetPacketLength(buffer);
		if (nPacketLen < 2 || (!m_bIsLocalServer && nPacketLen > (768 + 1024)))
		{
			OnDisconnect();
			return;
		}
		FreeArray(buffer);
		buffer = AllocArray(unsigned char, nPacketLen);
		asio::async_read(m_Socket,
			asio::buffer(buffer, nPacketLen),
			std::bind(&SocketBase::ProcessPacket,
				shared_from_this(), std::placeholders::_1, std::placeholders::_2, buffer, nPacketLen));
	}
	else
		OnDisconnect();
}

void SocketBase::ProcessPacket(const std::error_code &ec, std::size_t bytes_transferred, unsigned char* buffer, int nPacketLen)
{
	if (!ec)
	{
		unsigned short nBytes = static_cast<unsigned short>(bytes_transferred);

		if (!m_bIsLocalServer)
			WvsCrypto::Decrypt(buffer, m_aRecvIV, nBytes);
		InPacket iPacket(buffer, nBytes);
		try 
		{
			this->OnPacket(&iPacket);
		}
		catch (std::exception& ex) 
		{
			iPacket.RestorePacket();
			WvsLogger::LogFormat("Exceptions Occurred When Processing Packet (nType: %d), Excpetion Message: %s\nPacket Dump:\n", (int)iPacket.Decode2(), ex.what());
			iPacket.Print();
		}
		FreeArray(buffer);
		OnWaitingPacket();
	}
}

unsigned int SocketBase::GetSocketID() const
{
	return m_nSocketID;
}