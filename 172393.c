camel_imapx_server_can_use_idle (CamelIMAPXServer *is)
{
	gboolean use_idle;
	CamelIMAPXSettings *settings;

	g_mutex_lock (&is->priv->stream_lock);

	settings = camel_imapx_server_ref_settings (is);
	use_idle = camel_imapx_settings_get_use_idle (settings);
	g_object_unref (settings);

	/* Run IDLE if the server supports NOTIFY, to have
	   a constant read on the stream, thus to be notified. */
	if (!CAMEL_IMAPX_HAVE_CAPABILITY (is->priv->cinfo, NOTIFY) &&
	    !CAMEL_IMAPX_HAVE_CAPABILITY (is->priv->cinfo, IDLE)) {
		use_idle = FALSE;
	}

	g_mutex_unlock (&is->priv->stream_lock);

	return use_idle;
}