UINT DVCPluginEntry(IDRDYNVC_ENTRY_POINTS* pEntryPoints)
{
	UINT error = CHANNEL_RC_OK;
	VIDEO_PLUGIN* videoPlugin;
	VideoClientContext* videoContext;
	VideoClientContextPriv* priv;

	videoPlugin = (VIDEO_PLUGIN*)pEntryPoints->GetPlugin(pEntryPoints, "video");
	if (!videoPlugin)
	{
		videoPlugin = (VIDEO_PLUGIN*)calloc(1, sizeof(VIDEO_PLUGIN));
		if (!videoPlugin)
		{
			WLog_ERR(TAG, "calloc failed!");
			return CHANNEL_RC_NO_MEMORY;
		}

		videoPlugin->wtsPlugin.Initialize = video_plugin_initialize;
		videoPlugin->wtsPlugin.Connected = NULL;
		videoPlugin->wtsPlugin.Disconnected = NULL;
		videoPlugin->wtsPlugin.Terminated = video_plugin_terminated;

		videoContext = (VideoClientContext*)calloc(1, sizeof(VideoClientContext));
		if (!videoContext)
		{
			WLog_ERR(TAG, "calloc failed!");
			free(videoPlugin);
			return CHANNEL_RC_NO_MEMORY;
		}

		priv = VideoClientContextPriv_new(videoContext);
		if (!priv)
		{
			WLog_ERR(TAG, "VideoClientContextPriv_new failed!");
			free(videoContext);
			free(videoPlugin);
			return CHANNEL_RC_NO_MEMORY;
		}

		videoContext->handle = (void*)videoPlugin;
		videoContext->priv = priv;
		videoContext->timer = video_timer;
		videoContext->setGeometry = video_client_context_set_geometry;

		videoPlugin->wtsPlugin.pInterface = (void*)videoContext;
		videoPlugin->context = videoContext;

		error = pEntryPoints->RegisterPlugin(pEntryPoints, "video", (IWTSPlugin*)videoPlugin);
	}
	else
	{
		WLog_ERR(TAG, "could not get video Plugin.");
		return CHANNEL_RC_BAD_CHANNEL;
	}

	return error;
}