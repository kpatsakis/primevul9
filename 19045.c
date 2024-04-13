dnsc_shared_secret_cache_insert(struct slabhash *cache,
                                uint8_t key[DNSCRYPT_SHARED_SECRET_KEY_LENGTH],
                                uint32_t hash,
                                uint8_t nmkey[crypto_box_BEFORENMBYTES])
{
    struct shared_secret_cache_key* k =
        (struct shared_secret_cache_key*)calloc(1, sizeof(*k));
    uint8_t* d = malloc(crypto_box_BEFORENMBYTES);
    if(!k || !d) {
        free(k);
        free(d);
        return;
    }
    memcpy(d, nmkey, crypto_box_BEFORENMBYTES);
    lock_rw_init(&k->entry.lock);
    memcpy(k->key, key, DNSCRYPT_SHARED_SECRET_KEY_LENGTH);
    k->entry.hash = hash;
    k->entry.key = k;
    k->entry.data = d;
    slabhash_insert(cache,
                    hash, &k->entry,
                    d,
                    NULL);
}