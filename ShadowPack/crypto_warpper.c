#include "crypto_warpper.h"
#include "openssl\md5.h"
#include <string.h>
#include <crtdbg.h>
// ctr128不支持的算法（block不是16的）都改造成ctr128支持的算法，
// 为了加强安全性用md5，不然会出现两次加密ctr跳1只有后8字节变化，
// 是个漏洞，可能有更好的方法吧。。。
//
static void md5_sum(const unsigned char in[16], unsigned char out[16])
{
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, in, 16);
    MD5_Final(out, &ctx);
}
void Blowfish_Encrypt(const unsigned char in[16],
    unsigned char out[16], const void* key)
{
    md5_sum(in, out); // use md5 to harden encrypt...
    BF_encrypt((unsigned int *)out, (const BF_KEY *) key);
    BF_encrypt((unsigned int*)(out + 8), (const BF_KEY*)key);
    for (int i = 0; i < 8; i++) {
        out[8 + i] ^= out[i];
    }

}

void Cast_Encrypt(const unsigned char in[16],
    unsigned char out[16], const void* key)
{
    md5_sum(in, out);
    CAST_encrypt((CAST_LONG *)out, (const CAST_KEY *) key);
    CAST_encrypt((CAST_LONG*)(out + 8), (const CAST_KEY*)key);
    for (int i = 0; i < 8; i++) {
        out[8 + i] ^= out[i];
    }
    /*
    int i = 0;
    _CrtDbgReportW(_CRT_WARN, L"DBG", 1, L"Cipher", L"IN: %02hx %02hx %02hx %02hx %02hx %02hx %02hx %02hx\n",
        in[i], in[i + 1], in[i + 2], in[i + 3], in[i + 4], in[i + 5], in[i + 6], in[i + 7]);
    _CrtDbgReportW(_CRT_WARN, L"DBG", 1, L"Cipher", L"IN %02hx %02hx %02hx %02hx %02hx %02hx %02hx %02hx\n",
        in[i + 8], in[i + 9], in[i + 10], in[i + 11], in[i + 12], in[i + 13], in[i + 14], in[i + 15]);
    _CrtDbgReportW(_CRT_WARN, L"DBG", 1, L"Cipher", L"OUT: %02hx %02hx %02hx %02hx %02hx %02hx %02hx %02hx\n",
        out[i], out[i + 1], out[i + 2], out[i + 3], out[i + 4], out[i + 5], out[i + 6], out[i + 7]);
    _CrtDbgReportW(_CRT_WARN, L"DBG", 1, L"Cipher", L"OUT %02hx %02hx %02hx %02hx %02hx %02hx %02hx %02hx\n\n",
        out[i + 8], out[i + 9], out[i + 10], out[i + 11], out[i + 12], out[i + 13], out[i + 14], out[i + 15]);
        */
}

void Idea_Encrypt(const unsigned char in[16],
    unsigned char out[16], const void* key)
{
    IDEA_KEY_SCHEDULE idea_saved_key;
    md5_sum(in, out);
    memcpy(&idea_saved_key, key, sizeof(idea_saved_key));
    IDEA_encrypt((unsigned long*)out, &idea_saved_key);
    IDEA_encrypt((unsigned long*)(out + 8), &idea_saved_key);
    for (int i = 0; i < 8; i++) {
        out[8 + i] ^= out[i];
    }
}

//把流密码用成这样子，也是足够的诡异
void RC4_Encrypt(const unsigned char in[16],
    unsigned char out[16], const void* key)
{
    
    RC4_KEY rc4_saved_key;
    md5_sum(in, out);
    memcpy(&rc4_saved_key, key, sizeof(rc4_saved_key));
    RC4((RC4_KEY *)&rc4_saved_key, 16, in, out);
}

void TDES_Encrypt(const unsigned char in[16],
    unsigned char out[16], const void* key)
{
    TDES_KEY_T des_key;
    memcpy(&des_key, (const TDES_KEY_T*)(key), sizeof(des_key));
    md5_sum(in, out);

    DES_encrypt3((unsigned long*)out, &des_key.des1_key, &des_key.des2_key, &des_key.des3_key);
    DES_encrypt3((unsigned long*)(out + 8), &des_key.des1_key, &des_key.des2_key, &des_key.des3_key);
    for (int i = 0; i < 8; i++) {
        out[8 + i] ^= out[i];
    }
    
}