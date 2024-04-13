_wrap_gmac_aes256_set_key(void *_ctx, size_t len, const uint8_t * key)
{
	struct gmac_ctx *ctx = _ctx;

	if (unlikely(len != 32))
		abort();
	aes256_set_encrypt_key(&ctx->cipher.aes256, key);
	gcm_set_key(&ctx->key, &ctx->cipher, ctx->encrypt);
	ctx->pos = 0;
}