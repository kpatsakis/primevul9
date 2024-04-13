dnsc_nonces_lookup(struct slabhash* cache,
                   const uint8_t nonce[crypto_box_HALF_NONCEBYTES],
                   const uint8_t magic_query[DNSCRYPT_MAGIC_HEADER_LEN],
                   const uint8_t pk[crypto_box_PUBLICKEYBYTES],
                   uint32_t hash)
{
    struct nonce_cache_key k;
    memset(&k, 0, sizeof(k));
    k.entry.hash = hash;
    memcpy(k.nonce, nonce, crypto_box_HALF_NONCEBYTES);
    memcpy(k.magic_query, magic_query, DNSCRYPT_MAGIC_HEADER_LEN);
    memcpy(k.client_publickey, pk, crypto_box_PUBLICKEYBYTES);

    return slabhash_lookup(cache, hash, &k, 0);
}