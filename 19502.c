static VideoClientContextPriv* VideoClientContextPriv_new(VideoClientContext* video)
{
	VideoClientContextPriv* ret = calloc(1, sizeof(*ret));
	if (!ret)
		return NULL;

	ret->frames = Queue_New(TRUE, 10, 2);
	if (!ret->frames)
	{
		WLog_ERR(TAG, "unable to allocate frames queue");
		goto error_frames;
	}

	ret->surfacePool = BufferPool_New(FALSE, 0, 16);
	if (!ret->surfacePool)
	{
		WLog_ERR(TAG, "unable to create surface pool");
		goto error_surfacePool;
	}

	if (!InitializeCriticalSectionAndSpinCount(&ret->framesLock, 4 * 1000))
	{
		WLog_ERR(TAG, "unable to initialize frames lock");
		goto error_spinlock;
	}

	ret->video = video;

	/* don't set to unlimited so that we have the chance to send a feedback in
	 * the first second (for servers that want feedback directly)
	 */
	ret->lastSentRate = 30;
	return ret;

error_spinlock:
	BufferPool_Free(ret->surfacePool);
error_surfacePool:
	Queue_Free(ret->frames);
error_frames:
	free(ret);
	return NULL;
}