wrap_nettle_pbkdf2 (gnutls_mac_algorithm_t mac,
		    const void *key, size_t keysize,
		    const void *salt, size_t saltsize,
		    unsigned iter_count,
		    void *output, size_t length)
{
	struct nettle_mac_ctx ctx;
	int ret;

	ret = _mac_ctx_init(mac, &ctx);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ctx.set_key(&ctx, keysize, key);
	pbkdf2(&ctx.ctx, ctx.update, ctx.digest, ctx.length,
	       iter_count, saltsize, salt, length, output);

	return 0;
}