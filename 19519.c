static UINT video_control_send_presentation_response(VideoClientContext* context,
                                                     TSMM_PRESENTATION_RESPONSE* resp)
{
	BYTE buf[12];
	wStream* s;
	VIDEO_PLUGIN* video = (VIDEO_PLUGIN*)context->handle;
	IWTSVirtualChannel* channel;
	UINT ret;

	s = Stream_New(buf, 12);
	if (!s)
		return CHANNEL_RC_NO_MEMORY;

	Stream_Write_UINT32(s, 12);                                     /* cbSize */
	Stream_Write_UINT32(s, TSMM_PACKET_TYPE_PRESENTATION_RESPONSE); /* PacketType */
	Stream_Write_UINT8(s, resp->PresentationId);
	Stream_Zero(s, 3);
	Stream_SealLength(s);

	channel = video->control_callback->channel_callback->channel;
	ret = channel->Write(channel, 12, buf, NULL);
	Stream_Free(s, FALSE);

	return ret;
}