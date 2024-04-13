static int wrap_nettle_mac_init(gnutls_mac_algorithm_t algo, void **_ctx)
{
	struct nettle_mac_ctx *ctx;
	int ret;

	ctx = gnutls_calloc(1, sizeof(struct nettle_mac_ctx));
	if (ctx == NULL) {
		gnutls_assert();
		return GNUTLS_E_MEMORY_ERROR;
	}

	ctx->algo = algo;

	ret = _mac_ctx_init(algo, ctx);
	if (ret < 0) {
		gnutls_free(ctx);
		return gnutls_assert_val(ret);
	}

	*_ctx = ctx;

	return 0;
}