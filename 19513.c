static UINT video_plugin_terminated(IWTSPlugin* pPlugin)
{
	VIDEO_PLUGIN* video = (VIDEO_PLUGIN*)pPlugin;

	if (video->context)
		VideoClientContextPriv_free(video->context->priv);

	free(video->control_callback);
	free(video->data_callback);
	free(video->wtsPlugin.pInterface);
	free(pPlugin);
	return CHANNEL_RC_OK;
}