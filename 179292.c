camel_imapx_server_connect_sync (CamelIMAPXServer *is,
				 GCancellable *cancellable,
				 GError **error)
{
	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	if (is->priv->state == IMAPX_SHUTDOWN) {
		g_set_error (
			error, CAMEL_SERVICE_ERROR,
			CAMEL_SERVICE_ERROR_UNAVAILABLE,
			"Shutting down");
		return FALSE;
	}

	if (is->priv->state >= IMAPX_INITIALISED)
		return TRUE;

	is->priv->is_cyrus = FALSE;

	if (!imapx_reconnect (is, cancellable, error))
		return FALSE;

	g_mutex_lock (&is->priv->stream_lock);

	if (CAMEL_IMAPX_LACK_CAPABILITY (is->priv->cinfo, NAMESPACE)) {
		g_mutex_unlock (&is->priv->stream_lock);

		/* This also creates a needed faux NAMESPACE */
		if (!camel_imapx_server_list_sync (is, "INBOX", 0, cancellable, error))
			return FALSE;
	} else {
		g_mutex_unlock (&is->priv->stream_lock);
	}

	return TRUE;
}