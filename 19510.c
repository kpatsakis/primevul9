static UINT video_control_send_client_notification(VideoClientContext* context,
                                                   TSMM_CLIENT_NOTIFICATION* notif)
{
	BYTE buf[100];
	wStream* s;
	VIDEO_PLUGIN* video = (VIDEO_PLUGIN*)context->handle;
	IWTSVirtualChannel* channel;
	UINT ret;
	UINT32 cbSize;

	s = Stream_New(buf, 30);
	if (!s)
		return CHANNEL_RC_NO_MEMORY;

	cbSize = 16;
	Stream_Seek_UINT32(s);                                        /* cbSize */
	Stream_Write_UINT32(s, TSMM_PACKET_TYPE_CLIENT_NOTIFICATION); /* PacketType */
	Stream_Write_UINT8(s, notif->PresentationId);
	Stream_Write_UINT8(s, notif->NotificationType);
	Stream_Zero(s, 2);
	if (notif->NotificationType == TSMM_CLIENT_NOTIFICATION_TYPE_FRAMERATE_OVERRIDE)
	{
		Stream_Write_UINT32(s, 16); /* cbData */

		/* TSMM_CLIENT_NOTIFICATION_FRAMERATE_OVERRIDE */
		Stream_Write_UINT32(s, notif->FramerateOverride.Flags);
		Stream_Write_UINT32(s, notif->FramerateOverride.DesiredFrameRate);
		Stream_Zero(s, 4 * 2);

		cbSize += 4 * 4;
	}
	else
	{
		Stream_Write_UINT32(s, 0); /* cbData */
	}

	Stream_SealLength(s);
	Stream_SetPosition(s, 0);
	Stream_Write_UINT32(s, cbSize);
	Stream_Free(s, FALSE);

	channel = video->control_callback->channel_callback->channel;
	ret = channel->Write(channel, cbSize, buf, NULL);

	return ret;
}