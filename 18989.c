dnsc_shared_secrets_lookup(struct slabhash* cache,
                           uint8_t key[DNSCRYPT_SHARED_SECRET_KEY_LENGTH],
                           uint32_t hash)
{
    return slabhash_lookup(cache, hash, key, 0);
}