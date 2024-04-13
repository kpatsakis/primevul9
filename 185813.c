void rfx_context_free(RFX_CONTEXT* context)
{
	RFX_CONTEXT_PRIV* priv;

	if (!context)
		return;

	assert(NULL != context);
	assert(NULL != context->priv);
	assert(NULL != context->priv->TilePool);
	assert(NULL != context->priv->BufferPool);
	priv = context->priv;
	rfx_message_free(context, &context->currentMessage);
	free(context->quants);
	ObjectPool_Free(priv->TilePool);
	rfx_profiler_print(context);
	rfx_profiler_free(context);

	if (priv->UseThreads)
	{
		CloseThreadpool(context->priv->ThreadPool);
		DestroyThreadpoolEnvironment(&context->priv->ThreadPoolEnv);
		free(priv->workObjects);
		free(priv->tileWorkParams);
#ifdef WITH_PROFILER
		WLog_VRB(TAG,
		         "WARNING: Profiling results probably unusable with multithreaded RemoteFX codec!");
#endif
	}

	BufferPool_Free(context->priv->BufferPool);
	free(context->priv);
	free(context);
}