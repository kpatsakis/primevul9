static void VideoClientContextPriv_free(VideoClientContextPriv* priv)
{
	EnterCriticalSection(&priv->framesLock);
	while (Queue_Count(priv->frames))
	{
		VideoFrame* frame = Queue_Dequeue(priv->frames);
		if (frame)
			VideoFrame_free(&frame);
	}

	Queue_Free(priv->frames);
	LeaveCriticalSection(&priv->framesLock);

	DeleteCriticalSection(&priv->framesLock);

	if (priv->currentPresentation)
		PresentationContext_unref(priv->currentPresentation);

	BufferPool_Free(priv->surfacePool);
	free(priv);
}