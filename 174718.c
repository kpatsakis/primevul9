wrap_nettle_mac_set_key(void *_ctx, const void *key, size_t keylen)
{
	struct nettle_mac_ctx *ctx = _ctx;

	ctx->set_key(ctx->ctx_ptr, keylen, key);
	return 0;
}