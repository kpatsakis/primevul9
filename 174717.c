wrap_nettle_mac_update(void *_ctx, const void *text, size_t textsize)
{
	struct nettle_mac_ctx *ctx = _ctx;

	ctx->update(ctx->ctx_ptr, textsize, text);

	return GNUTLS_E_SUCCESS;
}