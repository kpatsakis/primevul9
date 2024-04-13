wrap_nettle_hkdf_expand (gnutls_mac_algorithm_t mac,
			 const void *key, size_t keysize,
			 const void *info, size_t infosize,
			 void *output, size_t length)
{
	struct nettle_mac_ctx ctx;
	int ret;

	ret = _mac_ctx_init(mac, &ctx);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ctx.set_key(&ctx, keysize, key);
	hkdf_expand(&ctx.ctx, ctx.update, ctx.digest, ctx.length,
		    infosize, info, length, output);

	return 0;
}