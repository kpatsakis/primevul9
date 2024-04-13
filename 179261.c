camel_imapx_server_can_use_idle (CamelIMAPXServer *is)
{
	gboolean use_idle = FALSE;

	g_mutex_lock (&is->priv->stream_lock);

	/* No need for IDLE if the server supports NOTIFY. */
	if (CAMEL_IMAPX_HAVE_CAPABILITY (is->priv->cinfo, NOTIFY)) {
		g_mutex_unlock (&is->priv->stream_lock);

		return FALSE;
	}

	if (CAMEL_IMAPX_HAVE_CAPABILITY (is->priv->cinfo, IDLE)) {
		CamelIMAPXSettings *settings;

		settings = camel_imapx_server_ref_settings (is);
		use_idle = camel_imapx_settings_get_use_idle (settings);
		g_object_unref (settings);
	}

	g_mutex_unlock (&is->priv->stream_lock);

	return use_idle;
}