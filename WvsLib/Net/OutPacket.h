#pragma once
#include <string>
#include <atomic>
#include <vector>

class InPacket;

class OutPacket
{
public:
	const static int
		DEFAULT_BUFF_SIZE = 256,
		THIS_PTR_OFFSET = 8, //8 bytes are reserved for 64-bit addressing
		HEADER_OFFSET = 4, //4 bytes are for packet header
		INITIAL_WRITE_INDEX = THIS_PTR_OFFSET + HEADER_OFFSET;

	class SharedPacket {
	private:
		friend class OutPacket;

		unsigned char* m_aBuff = nullptr;
		unsigned int m_nBuffSize = 0, m_nPacketSize = INITIAL_WRITE_INDEX;
		bool m_bBroadcasting = false;

		std::atomic<int> m_nRefCount;
		std::vector<void*> m_aBroadcasted;

	public:
		SharedPacket();
		~SharedPacket();

		void ToggleBroadcasting();
		bool IsBroadcasting() const;

		void IncRefCount();
		void DecRefCount();
		void AttachBroadcastingPacket(void* p);
	};

	void ExtendSize(int nExtendRate);

private:
	SharedPacket* m_pSharedPacket;

public:

	OutPacket();
	//OutPacket(short nOpcode);
	~OutPacket();

	int GetPacketSize() const
	{
		return m_pSharedPacket->m_nPacketSize - (INITIAL_WRITE_INDEX);
	}

	unsigned char* GetPacket()
	{
		return m_pSharedPacket->m_aBuff + (INITIAL_WRITE_INDEX); // +8 for this pointer
	}

	void CopyFromTransferedPacket(InPacket *oPacket);
	void Encode1(char value);
	void Encode2(short value);
	void Encode4(int value);
	void Encode8(long long int value);
	void EncodeBuffer(unsigned char *buff, int nSize, int nZero = 0);
	void EncodeStr(const std::string &str);

	/*Just for convenience.*/
	void EncodeHexString(const std::string& str);
	void Release();
	void Reset();

	/*Increase the reference count of the shared packet entry.*/
	void IncRefCount();

	/*Derease the reference count of the shared packet entry.*/
	void DecRefCount();
	SharedPacket* GetSharedPacket();

	void Print();
};

