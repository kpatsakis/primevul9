static int wrap_nettle_mac_fast(gnutls_mac_algorithm_t algo,
				const void *nonce, size_t nonce_size,
				const void *key, size_t key_size,
				const void *text, size_t text_size,
				void *digest)
{
	struct nettle_mac_ctx ctx;
	int ret;

	ret = _mac_ctx_init(algo, &ctx);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ctx.set_key(&ctx, key_size, key);
	if (ctx.set_nonce) {
		if (nonce == NULL || nonce_size == 0)
			return gnutls_assert_val(GNUTLS_E_INVALID_REQUEST);

		ctx.set_nonce(&ctx, nonce_size, nonce);
	}
	ctx.update(&ctx, text_size, text);
	ctx.digest(&ctx, ctx.length, digest);
	
	zeroize_temp_key(&ctx, sizeof(ctx));

	return 0;
}