#pragma once
#include "openssl/blowfish.h"
#include "openssl/cast.h"
#include "openssl/idea.h"
#include "openssl/rc4.h"
# ifdef  __cplusplus
extern "C" {
# endif

void Blowfish_Encrypt(const unsigned char in[16],
        unsigned char out[16], const void* key);

void Cast_Encrypt(const unsigned char in[16],
        unsigned char out[16], const void* key);

void Idea_Encrypt(const unsigned char in[16],
    unsigned char out[16], const void* key);

void RC4_Encrypt(const unsigned char in[16],
    unsigned char out[16], const void* key);

# ifdef  __cplusplus
}
# endif