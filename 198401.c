int sc_mutex_lock(const sc_context_t *ctx, void *mutex)
{
	if (ctx == NULL)
		return SC_ERROR_INVALID_ARGUMENTS;
	if (ctx->thread_ctx != NULL && ctx->thread_ctx->lock_mutex != NULL)
		return ctx->thread_ctx->lock_mutex(mutex);
	else
		return SC_SUCCESS;
}