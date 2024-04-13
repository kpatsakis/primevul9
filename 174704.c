static void _wrap_gmac_set_nonce(void *_ctx, size_t nonce_length, const uint8_t *nonce)
{
	struct gmac_ctx *ctx = _ctx;

	gcm_set_iv(&ctx->ctx, &ctx->key, nonce_length, nonce);
}