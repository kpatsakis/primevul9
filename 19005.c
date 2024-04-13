dnscrypt_server_uncurve(struct dnsc_env* env,
                        const dnsccert *cert,
                        uint8_t client_nonce[crypto_box_HALF_NONCEBYTES],
                        uint8_t nmkey[crypto_box_BEFORENMBYTES],
                        struct sldns_buffer* buffer)
{
    size_t len = sldns_buffer_limit(buffer);
    uint8_t *const buf = sldns_buffer_begin(buffer);
    uint8_t nonce[crypto_box_NONCEBYTES];
    struct dnscrypt_query_header *query_header;
    // shared secret cache
    uint8_t key[DNSCRYPT_SHARED_SECRET_KEY_LENGTH];
    struct lruhash_entry* entry;
    uint32_t hash;

    uint32_t nonce_hash;

    if (len <= DNSCRYPT_QUERY_HEADER_SIZE) {
        return -1;
    }

    query_header = (struct dnscrypt_query_header *)buf;

    /* Detect replay attacks */
    nonce_hash = dnsc_nonce_cache_key_hash(
        query_header->nonce,
        cert->magic_query,
        query_header->publickey);

    lock_basic_lock(&env->nonces_cache_lock);
    entry = dnsc_nonces_lookup(
        env->nonces_cache,
        query_header->nonce,
        cert->magic_query,
        query_header->publickey,
        nonce_hash);

    if(entry) {
        lock_rw_unlock(&entry->lock);
        env->num_query_dnscrypt_replay++;
        lock_basic_unlock(&env->nonces_cache_lock);
        return -1;
    }

    dnsc_nonce_cache_insert(
        env->nonces_cache,
        query_header->nonce,
        cert->magic_query,
        query_header->publickey,
        nonce_hash);
    lock_basic_unlock(&env->nonces_cache_lock);

    /* Find existing shared secret */
    hash = dnsc_shared_secrets_cache_key(key,
                                         cert->es_version[1],
                                         query_header->publickey,
                                         cert->keypair->crypt_secretkey);
    entry = dnsc_shared_secrets_lookup(env->shared_secrets_cache,
                                       key,
                                       hash);

    if(!entry) {
        lock_basic_lock(&env->shared_secrets_cache_lock);
        env->num_query_dnscrypt_secret_missed_cache++;
        lock_basic_unlock(&env->shared_secrets_cache_lock);
        if(cert->es_version[1] == 2) {
#ifdef USE_DNSCRYPT_XCHACHA20
            if (crypto_box_curve25519xchacha20poly1305_beforenm(
                        nmkey, query_header->publickey,
                        cert->keypair->crypt_secretkey) != 0) {
                return -1;
            }
#else
            return -1;
#endif
    } else {
        if (crypto_box_beforenm(nmkey,
                                query_header->publickey,
                                cert->keypair->crypt_secretkey) != 0) {
            return -1;
        }
    }
    // Cache the shared secret we just computed.
    dnsc_shared_secret_cache_insert(env->shared_secrets_cache,
                                    key,
                                    hash,
                                    nmkey);
    } else {
        /* copy shared secret and unlock entry */
        memcpy(nmkey, entry->data, crypto_box_BEFORENMBYTES);
        lock_rw_unlock(&entry->lock);
    }

    memcpy(nonce, query_header->nonce, crypto_box_HALF_NONCEBYTES);
    memset(nonce + crypto_box_HALF_NONCEBYTES, 0, crypto_box_HALF_NONCEBYTES);

    if(cert->es_version[1] == 2) {
#ifdef USE_DNSCRYPT_XCHACHA20
        if (crypto_box_curve25519xchacha20poly1305_open_easy_afternm
                (buf,
                buf + DNSCRYPT_QUERY_BOX_OFFSET,
                len - DNSCRYPT_QUERY_BOX_OFFSET, nonce,
                nmkey) != 0) {
            return -1;
        }
#else
        return -1;
#endif
    } else {
        if (crypto_box_open_easy_afternm
            (buf,
             buf + DNSCRYPT_QUERY_BOX_OFFSET,
             len - DNSCRYPT_QUERY_BOX_OFFSET, nonce,
             nmkey) != 0) {
            return -1;
        }
    }

    len -= DNSCRYPT_QUERY_HEADER_SIZE;

    while (*sldns_buffer_at(buffer, --len) == 0)
        ;

    if (*sldns_buffer_at(buffer, len) != 0x80) {
        return -1;
    }

    memcpy(client_nonce, nonce, crypto_box_HALF_NONCEBYTES);

    sldns_buffer_set_position(buffer, 0);
    sldns_buffer_set_limit(buffer, len);

    return 0;
}