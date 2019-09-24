#include "WzStreamCodec.h"
#include "WzArchive.h"
#include "WzMappedFileStream.h"
#include "WzAESKeyGen.h"
#include <codecvt>
#include <intrin.h> //For VS 2017.

static unsigned char aBasicKey[4] = { 0xB9, 0x7D, 0x63, 0xE9 };
static unsigned char aWzFileAESKey_TWMS[4096] = { 0 };
static unsigned char aWzFileAESKey_Empty[4096] = { 0 };
static unsigned char UserKey[] =
{
	0x13, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00,
	0x00, 0xB4, 0x00, 0x00, 0x00, 0x1B, 0x00, 0x00, 0x00, 0x0F, 0x00,
	0x00, 0x00, 0x33, 0x00, 0x00, 0x00, 0x52, 0x00, 0x00, 0x00
};

void WzStreamCodec::GenKey(unsigned char* aKey, unsigned char *aResult)
{
	WzAESKeyGen AESGen;
	unsigned char aIV[16];
	for (int i = 0; i < 16; i += 4) { memcpy(aIV + i, aKey, 4); }
	AESGen.SetParameters(256, 128);
	AESGen.StartEncryption(UserKey);
	AESGen.EncryptBlock(aIV, aResult);

	for (int i = 16; i < 4096; i += 16) 
		AESGen.EncryptBlock(aResult + i - 16, aResult + i);
}

WzStreamCodec::WzStreamCodec()
{
	//In the case, e.g. higher versions are not encrypted, just comment the line below.
	GenKey(aBasicKey, aWzFileAESKey_TWMS);
	Init();
}

WzStreamCodec::~WzStreamCodec()
{
}

void WzStreamCodec::Init()
{
#pragma warning(disable:4309)
	memcpy(aKeyStore[1], aWzFileAESKey_TWMS, 4096);

	__m128i amask = _mm_setr_epi8(
		0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9
	);
	__m128i aplus = _mm_set1_epi8(0x10);
	__m128i *ak0 = (__m128i *)aWzKey[0], *ak1 = (__m128i *)aWzKey[1];

	__m128i wmask = _mm_setr_epi16(
		0xAAAA, 0xAAAB, 0xAAAC, 0xAAAD, 0xAAAE, 0xAAAF, 0xAAA0, 0xAAA1
	);
	__m128i wplus = _mm_set1_epi16(0x0008);
	__m128i *wk0 = (__m128i *)aWideWzKey[0], *wk1 = (__m128i *)aWideWzKey[1];
	__m128i *k0 = (__m128i *)aKeyStore[0], *k1 = (__m128i *)aKeyStore[1];

	for (int i = 0x1000; i > 0; --i) 
	{
		_mm_storeu_si128(ak0, _mm_xor_si128(amask, _mm_loadu_si128(k0)));
		_mm_storeu_si128(ak1, _mm_xor_si128(amask, _mm_loadu_si128(k1)));
		_mm_storeu_si128(wk0, _mm_xor_si128(wmask, _mm_loadu_si128(k0)));
		_mm_storeu_si128(wk1, _mm_xor_si128(wmask, _mm_loadu_si128(k1)));
		++ak0, ++ak1;
		++wk0, ++wk1;
		++k0, ++k1;
		amask = _mm_add_epi8(amask, aplus);
		wmask = _mm_add_epi8(wmask, wplus);
	}
}

std::string WzStreamCodec::DecodeString(WzArchive *pArchive)
{
	static std::codecvt_utf8<char16_t> conv;
	static char in[0x10000];
	static char16_t ws[0x8000];
	static char ns[0x10000];
	int nLen = 0;

	pArchive->Read((char*)&nLen, 1);
	char cLen = ((char*)&nLen)[0];
	if (cLen > 0)
	{
		if (cLen == 127)
			pArchive->Read((char*)&nLen, 4);
		nLen *= 2;
		__m128i 
			*m1 = reinterpret_cast<__m128i *>(ws), 
			//Reading buffer from mapping file.
			*m2 = reinterpret_cast<__m128i *>(pArchive->GetStream()->GetStreamPtr()),
			*m3 = reinterpret_cast<__m128i *>(aWideWzKey[pArchive->Encrypted() ? 1 : 0]);

		for (int i = 0; i <= nLen >> 3; ++i)
			_mm_storeu_si128(m1 + i, _mm_xor_si128(_mm_loadu_si128(m2 + i), _mm_loadu_si128(m3 + i)));

		mbstate_t state;
		const char16_t * fnext;
		char * tnext;
		conv.out(state, ws, ws + nLen, fnext, ns, ns + 0x10000, tnext);
	}
	else
	{
		if (cLen == -128)
			pArchive->Read((char*)&nLen, 4);
		else
			nLen = cLen * -1;

		__m128i 
			*m1 = reinterpret_cast<__m128i *>(ns),
			//Reading buffer from mapping file.
			*m2 = reinterpret_cast<__m128i *>(pArchive->GetStream()->GetStreamPtr()),
			*m3 = reinterpret_cast<__m128i *>(aWzKey[pArchive->Encrypted() ? 1 : 0]);

		for (int i = 0; i <= nLen >> 4; ++i)
			_mm_storeu_si128(m1 + i, _mm_xor_si128(_mm_loadu_si128(m2 + i), _mm_loadu_si128(m3 + i)));
	}
	//Offset the stream position if you use MappingFile (Becuase there is no call to "Read").
	pArchive->GetStream()->SetPosition(pArchive->GetStream()->GetPosition() + nLen);

	ns[nLen] = 0;
	return std::string(ns, nLen);
}
