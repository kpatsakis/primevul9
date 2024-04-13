static void wrap_nettle_mac_deinit(void *hd)
{
	struct nettle_mac_ctx *ctx = hd;
	
	zeroize_temp_key(ctx, sizeof(*ctx));
	gnutls_free(ctx);
}