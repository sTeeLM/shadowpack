#pragma once

#include "openssl\aes.h"
#include "openssl\camellia.h"
#include "openssl\seed.h"
#include "openssl\md5.h"
#include "openssl\cast.h"
#include "openssl\modes.h"

#define CIPHER_BLOCK_SIZE (16)

class CPackCipher
{
public:
	typedef enum _PACK_CIPHER_TYPE_T{
		CIPHER_NONE,
		CIPHER_AES,
		CIPHER_SEED,
		CIPHER_CAMELLIA,
		CIPHER_CNT

	}PACK_CIPHER_TYPE_T;

	typedef union {
		AES_KEY aes_key;
		SEED_KEY_SCHEDULE seed_key;
		CAMELLIA_KEY camellia_key;
	} PACK_CIPHER_KEY_T;
public:
	CPackCipher(void);
	virtual ~CPackCipher(void);
	BOOL SetKeyType(PACK_CIPHER_TYPE_T type,LPCTSTR szPassword);
	void EncryptBlock(LPVOID pBuffer, size_t nSize, ULONGLONG nOffset);
	void EncryptBlock(LPVOID pBufferFrom, LPVOID pBufferTo, size_t nSize, ULONGLONG nOffset);
	void DecryptBlock(LPVOID pBuffer, size_t nSize, ULONGLONG nOffset);
	void DecryptBlock(LPVOID pBufferFrom, LPVOID pBufferTo, size_t nSize, ULONGLONG nOffset);
	PACK_CIPHER_TYPE_T GetCipherType(){return m_Type;}
private:
	void GenerateIV(BYTE iv[CIPHER_BLOCK_SIZE], ULONGLONG nIndex);
	void GenerateKey(LPCTSTR szPassword);
	void Crypt(LPVOID pBufferFrom, LPVOID pBufferTo, size_t nSize, BOOL bIsEncrypt, ULONGLONG nOffset);
private:
	BYTE m_Key[CIPHER_BLOCK_SIZE];
	PACK_CIPHER_TYPE_T m_Type;
	PACK_CIPHER_KEY_T m_CipherKey;
};
