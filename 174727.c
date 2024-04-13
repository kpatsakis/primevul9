wrap_nettle_hash_init(gnutls_digest_algorithm_t algo, void **_ctx)
{
	struct nettle_hash_ctx *ctx;
	int ret;

	ctx = gnutls_malloc(sizeof(struct nettle_hash_ctx));
	if (ctx == NULL) {
		gnutls_assert();
		return GNUTLS_E_MEMORY_ERROR;
	}

	ctx->algo = algo;

	if ((ret = _ctx_init(algo, ctx)) < 0) {
		gnutls_assert();
		gnutls_free(ctx);
		return ret;
	}

	*_ctx = ctx;

	return 0;
}