#ifndef CRYPTO_H
#define CRYPTO_H

bool initCrypto(void);

bool signHash(const uint8_t hash[32], uint8_t signature[64]);

bool getPublicKey(uint8_t publicKey[64]);

#endif