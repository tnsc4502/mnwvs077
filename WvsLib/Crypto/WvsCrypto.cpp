//
// by Buya (the developer's pseudonym)
//

#include "WvsCrypto.hpp"

#include <cstring>

#include "aes.h"

#include "..\Common\CryptoConstants.hpp"
#include "..\Common\ServerConstants.hpp"

namespace WvsCrypto
{

	unsigned char RrotateRight(unsigned char nVal, unsigned short nShifts)
	{
		nShifts &= 7;
		return static_cast<unsigned char>((nVal >> nShifts) | (nVal << (8 - nShifts)));
	}

	unsigned char RotateLeft(unsigned char nVal, unsigned short nShifts)
	{
		nShifts &= 7;
		return static_cast<unsigned char>((nVal << nShifts) | (nVal >> (8 - nShifts)));
	}

	void ShuffleIV(unsigned char * aIV)
	{
		unsigned char aNewIV[4] = { 0xF2, 0x53, 0x50, 0xC6 };
		unsigned char nInputIndex, cInputValue;
		unsigned int nIV, nShift;
		int nCounter = 0;

		for (; nCounter < 4; nCounter++)
		{
			nInputIndex = aIV[nCounter];
			cInputValue = CryptoConstants::kIvTable[nInputIndex];

			aNewIV[0] += (CryptoConstants::kIvTable[aNewIV[1]] - nInputIndex);
			aNewIV[1] -= (aNewIV[2] ^ cInputValue);
			aNewIV[2] ^= (CryptoConstants::kIvTable[aNewIV[3]] + nInputIndex);
			aNewIV[3] -= (aNewIV[0] - cInputValue);

			nIV = (aNewIV[3] << 24) | (aNewIV[2] << 16) | (aNewIV[1] << 8) | aNewIV[0];
			nShift = (nIV >> 0x1D) | (nIV << 0x03);

			aNewIV[0] = static_cast<unsigned char>(nShift & 0xFFu);
			aNewIV[1] = static_cast<unsigned char>((nShift >> 8) & 0xFFu);
			aNewIV[2] = static_cast<unsigned char>((nShift >> 16) & 0xFFu);
			aNewIV[3] = static_cast<unsigned char>((nShift >> 24) & 0xFFu);
		}

		// set iv
		memcpy(aIV, aNewIV, 4);
		memcpy(aIV + 4, aNewIV, 4);
		memcpy(aIV + 8, aNewIV, 4);
		memcpy(aIV + 12, aNewIV, 4);
	}

	void MultiplyBytes(unsigned char* aBufferOut, unsigned char* aBufferIn, int m, int n)
	{
		for (int i = 0; i < m * n; ++i)
			aBufferOut[i] = aBufferIn[i % 4];
	}

	void AESCrypt(unsigned char *aBuffer, unsigned char *aIV, unsigned short nSize)
	{
		unsigned char aTempIV[16];
		unsigned short nPOS = 0, nBlockPOS = 1456, nAmount = 0;

		aes_encrypt_ctx cx[1];
		aes_init();

		while (nSize > nPOS)
		{
			MultiplyBytes(aTempIV, aIV, 4, 4);
			aes_encrypt_key256(CryptoConstants::kAesKeys, cx);

			if (nSize > (nPOS + nBlockPOS))
				nAmount = nBlockPOS;
			else
				nAmount = nSize - nPOS;

			aes_ofb_crypt(aBuffer + nPOS, aBuffer + nPOS, nAmount, aTempIV, cx);
			nPOS += nBlockPOS;
			nBlockPOS = 1460;
		}
	}

	void Decrypt(unsigned char *aBuffer, unsigned char *aIV, unsigned short nSize)
	{
		AESCrypt(aBuffer, aIV, nSize);
		ShuffleIV(aIV);

		/*unsigned char a;
		unsigned char b;
		unsigned char c;
		unsigned short temp_size;
		int loop_counter = 0;

		for (; loop_counter < 3; ++loop_counter)
		{
			a = 0;
			b = 0;
			for (temp_size = size; temp_size > 0; --temp_size)
			{
				c = buffer[temp_size - 1];
				c = rotate_left(c, 3);
				c = c ^ 0x13;
				a = c;
				c = c ^ b;
				c = static_cast<unsigned char>(c - temp_size);
				c = rotate_right(c, 4);
				b = a;
				buffer[temp_size - 1] = c;
			}
			a = 0;
			b = 0;
			for (temp_size = size; temp_size > 0; --temp_size)
			{
				c = buffer[size - temp_size];
				c = c - 0x48;
				c = c ^ 0xFF;
				c = rotate_left(c, temp_size);
				a = c;
				c = c ^ b;
				c = static_cast<unsigned char>(c - temp_size);
				c = rotate_right(c, 3);
				b = a;
				buffer[size - temp_size] = c;
			}
		}*/
	}

	void Encrypt(unsigned char *aBuffer, unsigned char *aIV, unsigned short nSize)
	{
		/*unsigned char a;
		unsigned char c;
		unsigned short temp_size;
		int loop_counter = 0;

		for (; loop_counter < 3; ++loop_counter)
		{
			a = 0;
			for (temp_size = size; temp_size > 0; --temp_size)
			{
				c = buffer[size - temp_size];
				c = rotate_left(c, 3);
				c = static_cast<unsigned char>(c + temp_size);
				c = c ^ a;
				a = c;
				c = rotate_right(a, temp_size);
				c = c ^ 0xFF;
				c = c + 0x48;
				buffer[size - temp_size] = c;
			}
			a = 0;
			for (temp_size = size; temp_size > 0; --temp_size)
			{
				c = buffer[temp_size - 1];
				c = rotate_left(c, 4);
				c = static_cast<unsigned char>(c + temp_size);
				c = c ^ a;
				a = c;
				c = c ^ 0x13;
				c = rotate_right(c, 3);
				buffer[temp_size - 1] = c;
			}
		}*/

		AESCrypt(aBuffer, aIV, nSize);
		ShuffleIV(aIV);
	}

	void InitializeEncryption(unsigned char *aBuffer, unsigned char *aIV, unsigned short nSize)
	{

		unsigned short nVersion = (((aIV[3] << 8) | aIV[2]) ^ -(ServerConstants::kGameVersion + 1));
		nSize = nVersion ^ nSize;

		aBuffer[0] = nVersion & 0xFF;
		aBuffer[1] = (nVersion >> 8) & 0xFF;

		aBuffer[2] = nSize & 0xFF;
		aBuffer[3] = (nSize >> 8) & 0xFF;
	}

	unsigned short GetPacketLength(unsigned char *aBuffer)
	{
		return ((*(unsigned short *)(aBuffer)) ^ (*(unsigned short *)(aBuffer + 2)));
	}
}
