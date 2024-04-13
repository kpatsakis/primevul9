static void _md5_sha1_update(void *_ctx, size_t len, const uint8_t *data)
{
	struct md5_sha1_ctx *ctx = _ctx;

	md5_update(&ctx->md5, len, data);
	sha1_update(&ctx->sha1, len, data);
}