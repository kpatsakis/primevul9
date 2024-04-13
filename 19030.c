dnsc_apply_cfg(struct dnsc_env *env, struct config_file *cfg)
{
    if(dnsc_parse_certs(env, cfg) <= 0) {
        fatal_exit("dnsc_apply_cfg: no cert file loaded");
    }
    if(dnsc_parse_keys(env, cfg) <= 0) {
        fatal_exit("dnsc_apply_cfg: no key file loaded");
    }
    randombytes_buf(env->hash_key, sizeof env->hash_key);
    env->provider_name = cfg->dnscrypt_provider;

    if(dnsc_load_local_data(env, cfg) <= 0) {
        fatal_exit("dnsc_apply_cfg: could not load local data");
    }
    lock_basic_lock(&env->shared_secrets_cache_lock);
    env->shared_secrets_cache = slabhash_create(
        cfg->dnscrypt_shared_secret_cache_slabs,
        HASH_DEFAULT_STARTARRAY,
        cfg->dnscrypt_shared_secret_cache_size,
        dnsc_shared_secrets_sizefunc,
        dnsc_shared_secrets_compfunc,
        dnsc_shared_secrets_delkeyfunc,
        dnsc_shared_secrets_deldatafunc,
        NULL
    );
    lock_basic_unlock(&env->shared_secrets_cache_lock);
    if(!env->shared_secrets_cache){
        fatal_exit("dnsc_apply_cfg: could not create shared secrets cache.");
    }
    lock_basic_lock(&env->nonces_cache_lock);
    env->nonces_cache = slabhash_create(
        cfg->dnscrypt_nonce_cache_slabs,
        HASH_DEFAULT_STARTARRAY,
        cfg->dnscrypt_nonce_cache_size,
        dnsc_nonces_sizefunc,
        dnsc_nonces_compfunc,
        dnsc_nonces_delkeyfunc,
        dnsc_nonces_deldatafunc,
        NULL
    );
    lock_basic_unlock(&env->nonces_cache_lock);
    return 0;
}