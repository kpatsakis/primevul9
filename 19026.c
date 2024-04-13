dnsc_nonce_cache_key_hash(const uint8_t nonce[crypto_box_HALF_NONCEBYTES],
                          const uint8_t magic_query[DNSCRYPT_MAGIC_HEADER_LEN],
                          const uint8_t pk[crypto_box_PUBLICKEYBYTES])
{
    uint32_t h = 0;
    h = hashlittle(nonce, crypto_box_HALF_NONCEBYTES, h);
    h = hashlittle(magic_query, DNSCRYPT_MAGIC_HEADER_LEN, h);
    return hashlittle(pk, crypto_box_PUBLICKEYBYTES, h);
}