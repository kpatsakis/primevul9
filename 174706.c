wrap_nettle_hash_update(void *_ctx, const void *text, size_t textsize)
{
	struct nettle_hash_ctx *ctx = _ctx;

	ctx->update(ctx->ctx_ptr, textsize, text);

	return GNUTLS_E_SUCCESS;
}