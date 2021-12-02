#include "StdAfx.h"
#include "PackCipher.h"

CPackCipher::CPackCipher(void):
m_Type(CIPHER_NONE)
{

}

CPackCipher::~CPackCipher(void)
{

}

void CPackCipher::GenerateIV(BYTE iv[CIPHER_BLOCK_SIZE], ULONGLONG nIndex)
{
	memcpy(iv, &nIndex, 8); // 8 byte
	memcpy(iv + 8, m_Key, 8); // 8 byte

	for(INT i = 0 ; i < 8 ; i ++) { // swap to big endian
		iv[i] = iv[15 - i] ^ iv[i];
		iv[15 - i] = iv[15 - i] ^ iv[i];
		iv[i] = iv[15 - i] ^ iv[i];
	}
}

void CPackCipher::GenerateKey(LPCTSTR szPassword)
{
	MD5_CTX ctx;
	MD5_Init(&ctx);
	CT2CA szPassA(szPassword);

	::ZeroMemory(m_Key, sizeof(m_Key));

	MD5_Update(&ctx, szPassA, strlen(szPassA));
	MD5_Final(m_Key, &ctx);
}

BOOL CPackCipher::SetKeyType(pack_cipher_type_t type,LPCTSTR szPassword)
{
	if((NULL == szPassword || _tcslen(szPassword) == 0) && type != CIPHER_NONE)
		return FALSE;

	if(type != CIPHER_NONE) {
		// generate key
		GenerateKey(szPassword);
	}

	m_Type = type;

	switch (type) {
	case CIPHER_AES: 
		AES_set_encrypt_key(m_Key, 128, &m_CipherKey.aes_key);
		break;
	case CIPHER_SEED:
		SEED_set_key(m_Key, &m_CipherKey.seed_key);
		break;
	case CIPHER_CAMELLIA:
		Camellia_set_key(m_Key, 128, &m_CipherKey.camellia_key);
		break;
	case CIPHER_NONE:
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

void CPackCipher::Crypt(LPVOID pBuffer, size_t nSize, BOOL bIsEncrypt, ULONGLONG nOffset)
{

	// 根据计算nIndex计算block index
	ULONGLONG nBlockIndex = nOffset / CIPHER_BLOCK_SIZE;
	BYTE ecount_buf[CIPHER_BLOCK_SIZE];
	BYTE iv[CIPHER_BLOCK_SIZE];
	block128_f block = NULL;
	unsigned int num = 0;

	switch(m_Type) {
	case CIPHER_AES: 
		block = (block128_f)AES_encrypt;
		break;
	case CIPHER_SEED:
		block = (block128_f)SEED_encrypt;
		break;
	case CIPHER_CAMELLIA:
		block = (block128_f)Camellia_encrypt;
		break;
	}

	if(block == NULL) {
		return; // CIPHER_NONE
	}

	GenerateIV(iv, nBlockIndex);
	block(iv, ecount_buf, &m_CipherKey);

	num = (int)(nOffset % CIPHER_BLOCK_SIZE);

	CRYPTO_ctr128_encrypt((const unsigned char *)pBuffer, (unsigned char *)pBuffer, 
		nSize, &m_CipherKey, iv, ecount_buf, &num, block);
}

void CPackCipher::EncryptBlock(LPVOID pBuffer, size_t nSize, ULONGLONG nOffset)
{
	Crypt(pBuffer, nSize, TRUE, nOffset);
}

void CPackCipher::DecryptBlock(LPVOID pBuffer, size_t nSize, ULONGLONG nOffset)
{
	Crypt(pBuffer, nSize, FALSE, nOffset);
}
