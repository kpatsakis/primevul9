dnsc_delete(struct dnsc_env *env)
{
	if(!env) {
		return;
	}
	verbose(VERB_OPS, "DNSCrypt: Freeing environment.");
	sodium_free(env->signed_certs);
	sodium_free(env->rotated_certs);
	sodium_free(env->certs);
	sodium_free(env->keypairs);
	lock_basic_destroy(&env->shared_secrets_cache_lock);
	lock_basic_destroy(&env->nonces_cache_lock);
	slabhash_delete(env->shared_secrets_cache);
	slabhash_delete(env->nonces_cache);
	free(env);
}