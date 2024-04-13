static void *wrap_nettle_mac_copy(const void *_ctx)
{
	const struct nettle_mac_ctx *ctx = _ctx;
	struct nettle_mac_ctx *new_ctx;
	ptrdiff_t off = (uint8_t *)ctx->ctx_ptr - (uint8_t *)(&ctx->ctx);

	new_ctx = gnutls_calloc(1, sizeof(struct nettle_mac_ctx));
	if (new_ctx == NULL)
		return NULL;

	memcpy(new_ctx, ctx, sizeof(*ctx));
	new_ctx->ctx_ptr = (uint8_t *)&new_ctx->ctx + off;

	return new_ctx;
}