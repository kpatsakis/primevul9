unsigned long sc_thread_id(const sc_context_t *ctx)
{
	if (ctx == NULL || ctx->thread_ctx == NULL ||
	    ctx->thread_ctx->thread_id == NULL)
		return 0UL;
	else
		return ctx->thread_ctx->thread_id();
}