#include "pch.h"
#include "framework.h"
#include "PackCipher.h"

CPackCipher::CPackCipher(void):
m_Type(CIPHER_NONE)
{

}

CPackCipher::~CPackCipher(void)
{

}

LPCTSTR CPackCipher::m_CiherNames[] =
{
	_T("不加密"),
	_T("AES"),
	_T("SEED"),
	_T("CAMELLIA"),
	_T("BLOWFISH"),
	_T("CAST"),
	_T("IDEA"),
	_T("RC4"),
};

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

	ZeroMemory(m_Key, sizeof(m_Key));

	MD5_Update(&ctx, szPassA, strlen(szPassA));
	MD5_Final(m_Key, &ctx);
}

BOOL CPackCipher::SetKeyType(PACK_CIPHER_TYPE_T type,LPCTSTR szPassword)
{
	if((NULL == szPassword || _tcslen(szPassword) == 0) && type != CIPHER_NONE)
		return FALSE;

	if(type != CIPHER_NONE) {
		// generate key
		GenerateKey(szPassword);
	}

	m_strPassword = (szPassword == NULL ? _T("") : szPassword);

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
	case CIPHER_BLOWFISH:
		BF_set_key(&m_CipherKey.blowfish_key, 16, m_Key);
		break;
	case CIPHER_CAST:
		CAST_set_key(&m_CipherKey.cast_key, 16, m_Key);
		break;
	case CIPHER_IDEA:
		IDEA_set_encrypt_key(m_Key, &m_CipherKey.idea_key);
		break;
	case CIPHER_RC4:
		RC4_set_key(&m_CipherKey.rc4_key, 16, m_Key);
		break;
	case CIPHER_NONE:
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

void CPackCipher::Crypt(LPVOID pBufferFrom, LPVOID pBufferTo, size_t nSize, BOOL bIsEncrypt, ULONGLONG nOffset)
{
	// 根据计算nIndex计算block index
	ULONGLONG nBlockIndex = nOffset / CIPHER_BLOCK_SIZE;
	BYTE ecount_buf[CIPHER_BLOCK_SIZE];
	BYTE iv[CIPHER_BLOCK_SIZE];
	block128_f block = NULL;
	unsigned int num = 0;

	switch (m_Type) {
	case CIPHER_AES:
		block = (block128_f)AES_encrypt;
		break;
	case CIPHER_SEED:
		block = (block128_f)SEED_encrypt;
		break;
	case CIPHER_CAMELLIA:
		block = (block128_f)Camellia_encrypt;
		break;
	case CIPHER_BLOWFISH:
		block = (block128_f)Blowfish_Encrypt;
		break;
	case CIPHER_CAST:
		block = (block128_f)Cast_Encrypt;
		break;
	case CIPHER_IDEA:
		block = (block128_f)Idea_Encrypt;
		break;
	case CIPHER_RC4:
		block = (block128_f)RC4_Encrypt;
		break;
	}

	if (block == NULL) {
		CopyMemory(pBufferTo, pBufferFrom, nSize);
		return; // CIPHER_NONE
	}

	GenerateIV(iv, nBlockIndex);
	block(iv, ecount_buf, &m_CipherKey);

	num = (int)(nOffset % CIPHER_BLOCK_SIZE);

	CRYPTO_ctr128_encrypt((const unsigned char*)pBufferFrom, (unsigned char*)pBufferTo,
		nSize, &m_CipherKey, iv, ecount_buf, &num, block);
}

CString CPackCipher::GetPassword()
{
	return m_strPassword;
}

void CPackCipher::EncryptBlock(LPVOID pBuffer, size_t nSize, ULONGLONG nOffset)
{
	Crypt(pBuffer, pBuffer, nSize, TRUE, nOffset);
}

void CPackCipher::EncryptBlock(LPVOID pBufferFrom, LPVOID pBufferTo, size_t nSize, ULONGLONG nOffset)
{
	Crypt(pBufferFrom, pBufferTo, nSize, TRUE, nOffset);
}

void CPackCipher::DecryptBlock(LPVOID pBuffer, size_t nSize, ULONGLONG nOffset)
{
	Crypt(pBuffer, pBuffer, nSize, FALSE, nOffset);
}

void CPackCipher::DecryptBlock(LPVOID pBufferFrom, LPVOID pBufferTo, size_t nSize, ULONGLONG nOffset)
{
	Crypt(pBufferFrom, pBufferTo, nSize, FALSE, nOffset);
}

UINT CPackCipher::GetCipherCount()
{
	return CIPHER_CNT;
}

LPCTSTR CPackCipher::GetCipherName(UINT nIndex)
{
	if (nIndex < CIPHER_CNT) {
		return m_CiherNames[nIndex];
	}
	return _T("");
}