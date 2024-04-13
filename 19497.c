static UINT video_data_on_new_channel_connection(IWTSListenerCallback* pListenerCallback,
                                                 IWTSVirtualChannel* pChannel, BYTE* Data,
                                                 BOOL* pbAccept,
                                                 IWTSVirtualChannelCallback** ppCallback)
{
	VIDEO_CHANNEL_CALLBACK* callback;
	VIDEO_LISTENER_CALLBACK* listener_callback = (VIDEO_LISTENER_CALLBACK*)pListenerCallback;

	WINPR_UNUSED(Data);
	WINPR_UNUSED(pbAccept);

	callback = (VIDEO_CHANNEL_CALLBACK*)calloc(1, sizeof(VIDEO_CHANNEL_CALLBACK));
	if (!callback)
	{
		WLog_ERR(TAG, "calloc failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	callback->iface.OnDataReceived = video_data_on_data_received;
	callback->iface.OnClose = video_data_on_close;
	callback->plugin = listener_callback->plugin;
	callback->channel_mgr = listener_callback->channel_mgr;
	callback->channel = pChannel;
	listener_callback->channel_callback = callback;

	*ppCallback = (IWTSVirtualChannelCallback*)callback;

	return CHANNEL_RC_OK;
}