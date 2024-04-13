static UINT video_plugin_initialize(IWTSPlugin* plugin, IWTSVirtualChannelManager* channelMgr)
{
	UINT status;
	VIDEO_PLUGIN* video = (VIDEO_PLUGIN*)plugin;
	VIDEO_LISTENER_CALLBACK* callback;

	video->control_callback = callback =
	    (VIDEO_LISTENER_CALLBACK*)calloc(1, sizeof(VIDEO_LISTENER_CALLBACK));
	if (!callback)
	{
		WLog_ERR(TAG, "calloc for control callback failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	callback->iface.OnNewChannelConnection = video_control_on_new_channel_connection;
	callback->plugin = plugin;
	callback->channel_mgr = channelMgr;

	status = channelMgr->CreateListener(channelMgr, VIDEO_CONTROL_DVC_CHANNEL_NAME, 0,
	                                    (IWTSListenerCallback*)callback, &(video->controlListener));

	if (status != CHANNEL_RC_OK)
		return status;
	video->controlListener->pInterface = video->wtsPlugin.pInterface;

	video->data_callback = callback =
	    (VIDEO_LISTENER_CALLBACK*)calloc(1, sizeof(VIDEO_LISTENER_CALLBACK));
	if (!callback)
	{
		WLog_ERR(TAG, "calloc for data callback failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	callback->iface.OnNewChannelConnection = video_data_on_new_channel_connection;
	callback->plugin = plugin;
	callback->channel_mgr = channelMgr;

	status = channelMgr->CreateListener(channelMgr, VIDEO_DATA_DVC_CHANNEL_NAME, 0,
	                                    (IWTSListenerCallback*)callback, &(video->dataListener));

	if (status == CHANNEL_RC_OK)
		video->dataListener->pInterface = video->wtsPlugin.pInterface;

	return status;
}