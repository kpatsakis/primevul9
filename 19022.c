dnsc_shared_secrets_cache_key(uint8_t* key,
                              uint8_t esversion,
                              uint8_t* pk,
                              uint8_t* sk)
{
    key[0] = esversion;
    memcpy(key + 1, pk, crypto_box_PUBLICKEYBYTES);
    memcpy(key + 1 + crypto_box_PUBLICKEYBYTES, sk, crypto_box_SECRETKEYBYTES);
    return hashlittle(key, DNSCRYPT_SHARED_SECRET_KEY_LENGTH, 0);
}