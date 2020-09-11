//
// by Buya (the developer's pseudonym)
//

#pragma once

namespace WvsCrypto
{
	void Decrypt(unsigned char *buffer, unsigned char *iv, unsigned short size);
	void Encrypt(unsigned char *buffer, unsigned char *iv, unsigned short size);
	void InitializeEncryption(unsigned char *buffer, unsigned char *iv, unsigned short size);
	unsigned short GetPacketLength(unsigned char *buffer);
}
