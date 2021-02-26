#pragma once

#include "openssl\aes.h"
#include "openssl\camellia.h"
#include "openssl\seed.h"
#include "openssl\md5.h"
#include "openssl\modes.h"

#define CIPHER_BLOCK_SIZE (16)

class CPackCipher
{
public:
	typedef enum {
		CIPHER_NONE,
		CIPHER_AES,
		CIPHER_SEED,
		CIPHER_CAMELLIA,
		CIPHER_CNT

	}pack_cipher_type_t;

	typedef union {
		AES_KEY aes_key;
		SEED_KEY_SCHEDULE seed_key;
		CAMELLIA_KEY camellia_key;
	} cipher_key_t;
public:
	CPackCipher(void);
	virtual ~CPackCipher(void);
	BOOL SetKeyType(pack_cipher_type_t type,LPCTSTR szPassword);
	void EncryptBlock(LPVOID pBuffer, size_t nSize, ULONGLONG nOffset);
	void DecryptBlock(LPVOID pBuffer, size_t nSize, ULONGLONG nOffset);
	pack_cipher_type_t GetCipherType(){return m_Type;}
private:
	void GenerateIV(BYTE iv[CIPHER_BLOCK_SIZE], ULONGLONG nIndex);
	void GenerateKey(LPCTSTR szPassword);
	void Crypt(LPVOID pBuffer, size_t nSize, BOOL bIsEncrypt, ULONGLONG nOffset);
private:
	BYTE m_Key[CIPHER_BLOCK_SIZE];
	pack_cipher_type_t m_Type;
	cipher_key_t m_CipherKey;
};
