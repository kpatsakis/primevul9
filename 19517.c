static UINT video_VideoData(VideoClientContext* context, TSMM_VIDEO_DATA* data)
{
	VideoClientContextPriv* priv = context->priv;
	PresentationContext* presentation;
	int status;

	presentation = priv->currentPresentation;
	if (!presentation)
	{
		WLog_ERR(TAG, "no current presentation");
		return CHANNEL_RC_OK;
	}

	if (presentation->PresentationId != data->PresentationId)
	{
		WLog_ERR(TAG, "current presentation id=%d doesn't match data id=%d",
		         presentation->PresentationId, data->PresentationId);
		return CHANNEL_RC_OK;
	}

	if (!Stream_EnsureRemainingCapacity(presentation->currentSample, data->cbSample))
	{
		WLog_ERR(TAG, "unable to expand the current packet");
		return CHANNEL_RC_NO_MEMORY;
	}

	Stream_Write(presentation->currentSample, data->pSample, data->cbSample);

	if (data->CurrentPacketIndex == data->PacketsInSample)
	{
		H264_CONTEXT* h264 = presentation->h264;
		UINT64 startTime = GetTickCount64(), timeAfterH264;
		MAPPED_GEOMETRY* geom = presentation->geometry;

		Stream_SealLength(presentation->currentSample);
		Stream_SetPosition(presentation->currentSample, 0);

		status = h264->subsystem->Decompress(h264, Stream_Pointer(presentation->currentSample),
		                                     Stream_Length(presentation->currentSample));
		if (status == 0)
			return CHANNEL_RC_OK;

		if (status < 0)
			return CHANNEL_RC_OK;

		timeAfterH264 = GetTickCount64();
		if (data->SampleNumber == 1)
		{
			presentation->lastPublishTime = startTime;
		}

		presentation->lastPublishTime += (data->hnsDuration / 10000);
		if (presentation->lastPublishTime <= timeAfterH264 + 10)
		{
			int dropped = 0;

			/* if the frame is to be published in less than 10 ms, let's consider it's now */
			yuv_to_rgb(presentation, presentation->surfaceData);

			context->showSurface(context, presentation->surface);

			priv->publishedFrames++;

			/* cleanup previously scheduled frames */
			EnterCriticalSection(&priv->framesLock);
			while (Queue_Count(priv->frames) > 0)
			{
				VideoFrame* frame = Queue_Dequeue(priv->frames);
				if (frame)
				{
					priv->droppedFrames++;
					VideoFrame_free(&frame);
					dropped++;
				}
			}
			LeaveCriticalSection(&priv->framesLock);

			if (dropped)
				WLog_DBG(TAG, "showing frame (%d dropped)", dropped);
		}
		else
		{
			BOOL enqueueResult;
			VideoFrame* frame = calloc(1, sizeof(*frame));
			if (!frame)
			{
				WLog_ERR(TAG, "unable to create frame");
				return CHANNEL_RC_NO_MEMORY;
			}
			mappedGeometryRef(geom);

			frame->presentation = presentation;
			frame->publishTime = presentation->lastPublishTime;
			frame->geometry = geom;
			frame->w = presentation->SourceWidth;
			frame->h = presentation->SourceHeight;

			frame->surfaceData = BufferPool_Take(priv->surfacePool, frame->w * frame->h * 4);
			if (!frame->surfaceData)
			{
				WLog_ERR(TAG, "unable to allocate frame data");
				mappedGeometryUnref(geom);
				free(frame);
				return CHANNEL_RC_NO_MEMORY;
			}

			if (!yuv_to_rgb(presentation, frame->surfaceData))
			{
				WLog_ERR(TAG, "error during YUV->RGB conversion");
				BufferPool_Return(priv->surfacePool, frame->surfaceData);
				mappedGeometryUnref(geom);
				free(frame);
				return CHANNEL_RC_NO_MEMORY;
			}

			InterlockedIncrement(&presentation->refCounter);

			EnterCriticalSection(&priv->framesLock);
			enqueueResult = Queue_Enqueue(priv->frames, frame);
			LeaveCriticalSection(&priv->framesLock);

			if (!enqueueResult)
			{
				WLog_ERR(TAG, "unable to enqueue frame");
				VideoFrame_free(&frame);
				return CHANNEL_RC_NO_MEMORY;
			}

			WLog_DBG(TAG, "scheduling frame in %" PRIu32 " ms", (frame->publishTime - startTime));
		}
	}

	return CHANNEL_RC_OK;
}