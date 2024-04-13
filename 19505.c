static void video_timer(VideoClientContext* video, UINT64 now)
{
	PresentationContext* presentation;
	VideoClientContextPriv* priv = video->priv;
	VideoFrame *peekFrame, *frame = NULL;

	EnterCriticalSection(&priv->framesLock);
	do
	{
		peekFrame = (VideoFrame*)Queue_Peek(priv->frames);
		if (!peekFrame)
			break;

		if (peekFrame->publishTime > now)
			break;

		if (frame)
		{
			WLog_DBG(TAG, "dropping frame @%" PRIu64, frame->publishTime);
			priv->droppedFrames++;
			VideoFrame_free(&frame);
		}
		frame = peekFrame;
		Queue_Dequeue(priv->frames);
	} while (1);
	LeaveCriticalSection(&priv->framesLock);

	if (!frame)
		goto treat_feedback;

	presentation = frame->presentation;

	priv->publishedFrames++;
	memcpy(presentation->surfaceData, frame->surfaceData, frame->w * frame->h * 4);

	video->showSurface(video, presentation->surface);

	VideoFrame_free(&frame);

treat_feedback:
	if (priv->nextFeedbackTime < now)
	{
		/* we can compute some feedback only if we have some published frames and
		 * a current presentation
		 */
		if (priv->publishedFrames && priv->currentPresentation)
		{
			UINT32 computedRate;

			InterlockedIncrement(&priv->currentPresentation->refCounter);

			if (priv->droppedFrames)
			{
				/**
				 * some dropped frames, looks like we're asking too many frames per seconds,
				 * try lowering rate. We go directly from unlimited rate to 24 frames/seconds
				 * otherwise we lower rate by 2 frames by seconds
				 */
				if (priv->lastSentRate == XF_VIDEO_UNLIMITED_RATE)
					computedRate = 24;
				else
				{
					computedRate = priv->lastSentRate - 2;
					if (!computedRate)
						computedRate = 2;
				}
			}
			else
			{
				/**
				 * we treat all frames ok, so either ask the server to send more,
				 * or stay unlimited
				 */
				if (priv->lastSentRate == XF_VIDEO_UNLIMITED_RATE)
					computedRate = XF_VIDEO_UNLIMITED_RATE; /* stay unlimited */
				else
				{
					computedRate = priv->lastSentRate + 2;
					if (computedRate > XF_VIDEO_UNLIMITED_RATE)
						computedRate = XF_VIDEO_UNLIMITED_RATE;
				}
			}

			if (computedRate != priv->lastSentRate)
			{
				TSMM_CLIENT_NOTIFICATION notif;
				notif.PresentationId = priv->currentPresentation->PresentationId;
				notif.NotificationType = TSMM_CLIENT_NOTIFICATION_TYPE_FRAMERATE_OVERRIDE;
				if (computedRate == XF_VIDEO_UNLIMITED_RATE)
				{
					notif.FramerateOverride.Flags = 0x01;
					notif.FramerateOverride.DesiredFrameRate = 0x00;
				}
				else
				{
					notif.FramerateOverride.Flags = 0x02;
					notif.FramerateOverride.DesiredFrameRate = computedRate;
				}

				video_control_send_client_notification(video, &notif);
				priv->lastSentRate = computedRate;

				WLog_DBG(TAG, "server notified with rate %d published=%d dropped=%d",
				         priv->lastSentRate, priv->publishedFrames, priv->droppedFrames);
			}

			PresentationContext_unref(priv->currentPresentation);
		}

		WLog_DBG(TAG, "currentRate=%d published=%d dropped=%d", priv->lastSentRate,
		         priv->publishedFrames, priv->droppedFrames);

		priv->droppedFrames = 0;
		priv->publishedFrames = 0;
		priv->nextFeedbackTime = now + 1000;
	}
}