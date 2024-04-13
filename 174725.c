static void _wrap_gmac_digest(void *_ctx, size_t length, uint8_t *digest)
{
	struct gmac_ctx *ctx = _ctx;

	if (ctx->pos)
		gcm_update(&ctx->ctx, &ctx->key, ctx->pos, ctx->buffer);
	gcm_digest(&ctx->ctx, &ctx->key, &ctx->cipher, ctx->encrypt, length, digest);
	ctx->pos = 0;
}