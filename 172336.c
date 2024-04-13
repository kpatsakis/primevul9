camel_imapx_server_disconnect_sync (CamelIMAPXServer *is,
				    GCancellable *cancellable,
				    GError **error)
{
	GCancellable *idle_cancellable;
	gboolean success = TRUE;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	g_mutex_lock (&is->priv->idle_lock);
	idle_cancellable = is->priv->idle_cancellable;
	if (idle_cancellable)
		g_object_ref (idle_cancellable);
	g_mutex_unlock (&is->priv->idle_lock);

	if (idle_cancellable)
		g_cancellable_cancel (idle_cancellable);
	g_clear_object (&idle_cancellable);

	g_mutex_lock (&is->priv->stream_lock);
	if (is->priv->connection) {
		/* No need to wait for close for too long */
		imapx_server_set_connection_timeout (is->priv->connection, 3);
	}
	g_mutex_unlock (&is->priv->stream_lock);

	/* Ignore errors here. */
	camel_imapx_server_stop_idle_sync (is, cancellable, NULL);

	g_mutex_lock (&is->priv->stream_lock);
	if (is->priv->connection)
		success = g_io_stream_close (is->priv->connection, cancellable, error);
	g_mutex_unlock (&is->priv->stream_lock);

	imapx_disconnect (is);

	return success;
}