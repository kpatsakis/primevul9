static UINT video_control_on_close(IWTSVirtualChannelCallback* pChannelCallback)
{
	free(pChannelCallback);
	return CHANNEL_RC_OK;
}