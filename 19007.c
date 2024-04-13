dnsc_create(void)
{
	struct dnsc_env *env;
#ifdef SODIUM_MISUSE_HANDLER
	sodium_set_misuse_handler(sodium_misuse_handler);
#endif
	if (sodium_init() == -1) {
		fatal_exit("dnsc_create: could not initialize libsodium.");
	}
	env = (struct dnsc_env *) calloc(1, sizeof(struct dnsc_env));
	lock_basic_init(&env->shared_secrets_cache_lock);
	lock_protect(&env->shared_secrets_cache_lock,
                 &env->num_query_dnscrypt_secret_missed_cache,
                 sizeof(env->num_query_dnscrypt_secret_missed_cache));
	lock_basic_init(&env->nonces_cache_lock);
	lock_protect(&env->nonces_cache_lock,
                 &env->nonces_cache,
                 sizeof(env->nonces_cache));
	lock_protect(&env->nonces_cache_lock,
                 &env->num_query_dnscrypt_replay,
                 sizeof(env->num_query_dnscrypt_replay));

	return env;
}