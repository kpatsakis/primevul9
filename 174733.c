static void _wrap_gmac_update(void *_ctx, size_t length, const uint8_t *data)
{
	struct gmac_ctx *ctx = _ctx;

	if (ctx->pos + length < GCM_BLOCK_SIZE) {
		memcpy(&ctx->buffer[ctx->pos], data, length);
		ctx->pos += length;
		return;
	}

	if (ctx->pos) {
		memcpy(&ctx->buffer[ctx->pos], data, GCM_BLOCK_SIZE - ctx->pos);
		gcm_update(&ctx->ctx, &ctx->key, GCM_BLOCK_SIZE, ctx->buffer);
		data += GCM_BLOCK_SIZE - ctx->pos;
		length -= GCM_BLOCK_SIZE - ctx->pos;
	}

	if (length >= GCM_BLOCK_SIZE) {
		gcm_update(&ctx->ctx, &ctx->key,
			   length / GCM_BLOCK_SIZE * GCM_BLOCK_SIZE,
			   data);
		data += length / GCM_BLOCK_SIZE * GCM_BLOCK_SIZE;
		length %= GCM_BLOCK_SIZE;
	}

	memcpy(ctx->buffer, data, length);
	ctx->pos = length;
}