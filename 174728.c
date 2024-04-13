wrap_nettle_hkdf_extract (gnutls_mac_algorithm_t mac,
			  const void *key, size_t keysize,
			  const void *salt, size_t saltsize,
			  void *output)
{
	struct nettle_mac_ctx ctx;
	int ret;

	ret = _mac_ctx_init(mac, &ctx);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ctx.set_key(&ctx, saltsize, salt);
	hkdf_extract(&ctx.ctx, ctx.update, ctx.digest, ctx.length,
		     keysize, key, output);

	return 0;
}