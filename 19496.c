static UINT video_control_on_data_received(IWTSVirtualChannelCallback* pChannelCallback, wStream* s)
{
	VIDEO_CHANNEL_CALLBACK* callback = (VIDEO_CHANNEL_CALLBACK*)pChannelCallback;
	VIDEO_PLUGIN* video;
	VideoClientContext* context;
	UINT ret = CHANNEL_RC_OK;
	UINT32 cbSize, packetType;

	video = (VIDEO_PLUGIN*)callback->plugin;
	context = (VideoClientContext*)video->wtsPlugin.pInterface;

	if (Stream_GetRemainingLength(s) < 4)
		return ERROR_INVALID_DATA;

	Stream_Read_UINT32(s, cbSize);
	if (cbSize < 8 || Stream_GetRemainingLength(s) < (cbSize - 4))
	{
		WLog_ERR(TAG, "invalid cbSize");
		return ERROR_INVALID_DATA;
	}

	Stream_Read_UINT32(s, packetType);
	switch (packetType)
	{
		case TSMM_PACKET_TYPE_PRESENTATION_REQUEST:
			ret = video_read_tsmm_presentation_req(context, s);
			break;
		default:
			WLog_ERR(TAG, "not expecting packet type %" PRIu32 "", packetType);
			ret = ERROR_UNSUPPORTED_TYPE;
			break;
	}

	return ret;
}