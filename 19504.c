static UINT video_data_on_close(IWTSVirtualChannelCallback* pChannelCallback)
{
	free(pChannelCallback);
	return CHANNEL_RC_OK;
}