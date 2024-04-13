imapx_server_inactivity_thread (gpointer user_data)
{
	CamelIMAPXServer *is = user_data;
	GError *local_error = NULL;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), NULL);

	if (camel_imapx_server_is_in_idle (is)) {
		/* Stop and restart the IDLE command. */
		if (!camel_imapx_server_schedule_idle_sync (is, NULL, is->priv->cancellable, &local_error) &&
		    !g_error_matches (local_error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
			camel_imapx_debug (io, camel_imapx_server_get_tagprefix (is),
				"%s: Failed to restart IDLE: %s\n", G_STRFUNC, local_error ? local_error->message : "Unknown error");
	} else {
		if (!camel_imapx_server_noop_sync (is, NULL, is->priv->cancellable, &local_error) &&
		    !g_error_matches (local_error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
			camel_imapx_debug (io, camel_imapx_server_get_tagprefix (is),
				"%s: Failed to issue NOOP: %s\n", G_STRFUNC, local_error ? local_error->message : "Unknown error");
	}

	g_clear_error (&local_error);
	g_object_unref (is);

	return NULL;
}