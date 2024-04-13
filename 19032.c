dnsc_nonce_cache_insert(struct slabhash *cache,
                        const uint8_t nonce[crypto_box_HALF_NONCEBYTES],
                        const uint8_t magic_query[DNSCRYPT_MAGIC_HEADER_LEN],
                        const uint8_t pk[crypto_box_PUBLICKEYBYTES],
                        uint32_t hash)
{
    struct nonce_cache_key* k =
        (struct nonce_cache_key*)calloc(1, sizeof(*k));
    if(!k) {
        free(k);
        return;
    }
    lock_rw_init(&k->entry.lock);
    memcpy(k->nonce, nonce, crypto_box_HALF_NONCEBYTES);
    memcpy(k->magic_query, magic_query, DNSCRYPT_MAGIC_HEADER_LEN);
    memcpy(k->client_publickey, pk, crypto_box_PUBLICKEYBYTES);
    k->entry.hash = hash;
    k->entry.key = k;
    k->entry.data = NULL;
    slabhash_insert(cache,
                    hash, &k->entry,
                    NULL,
                    NULL);
}