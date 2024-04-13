static void bio_post_read_processing(struct bio_post_read_ctx *ctx)
{
	switch (++ctx->cur_step) {
	case STEP_DECRYPT:
		if (ctx->enabled_steps & (1 << STEP_DECRYPT)) {
			INIT_WORK(&ctx->work, decrypt_work);
			fscrypt_enqueue_decrypt_work(&ctx->work);
			return;
		}
		ctx->cur_step++;
		/* fall-through */
	default:
		__read_end_io(ctx->bio);
	}
}