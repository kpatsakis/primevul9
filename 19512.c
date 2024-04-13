static UINT video_data_on_data_received(IWTSVirtualChannelCallback* pChannelCallback, wStream* s)
{
	VIDEO_CHANNEL_CALLBACK* callback = (VIDEO_CHANNEL_CALLBACK*)pChannelCallback;
	VIDEO_PLUGIN* video;
	VideoClientContext* context;
	UINT32 cbSize, packetType;
	TSMM_VIDEO_DATA data;

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
	if (packetType != TSMM_PACKET_TYPE_VIDEO_DATA)
	{
		WLog_ERR(TAG, "only expecting VIDEO_DATA on the data channel");
		return ERROR_INVALID_DATA;
	}

	if (Stream_GetRemainingLength(s) < 32)
	{
		WLog_ERR(TAG, "not enough bytes for a TSMM_VIDEO_DATA");
		return ERROR_INVALID_DATA;
	}

	Stream_Read_UINT8(s, data.PresentationId);
	Stream_Read_UINT8(s, data.Version);
	Stream_Read_UINT8(s, data.Flags);
	Stream_Seek_UINT8(s); /* reserved */
	Stream_Read_UINT64(s, data.hnsTimestamp);
	Stream_Read_UINT64(s, data.hnsDuration);
	Stream_Read_UINT16(s, data.CurrentPacketIndex);
	Stream_Read_UINT16(s, data.PacketsInSample);
	Stream_Read_UINT32(s, data.SampleNumber);
	Stream_Read_UINT32(s, data.cbSample);
	data.pSample = Stream_Pointer(s);

	/*
	    WLog_DBG(TAG, "videoData: id:%"PRIu8" version:%"PRIu8" flags:0x%"PRIx8" timestamp=%"PRIu64"
	   duration=%"PRIu64 " curPacketIndex:%"PRIu16" packetInSample:%"PRIu16" sampleNumber:%"PRIu32"
	   cbSample:%"PRIu32"", data.PresentationId, data.Version, data.Flags, data.hnsTimestamp,
	   data.hnsDuration, data.CurrentPacketIndex, data.PacketsInSample, data.SampleNumber,
	   data.cbSample);
	*/

	return video_VideoData(context, &data);
}