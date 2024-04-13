data_available (GIOChannel *source,
                GIOCondition condition,
                gpointer data)
{
	NMVPNCPlugin *plugin = NM_VPNC_PLUGIN (data);
	NMVPNCPluginPrivate *priv = NM_VPNC_PLUGIN_GET_PRIVATE (plugin);
	GError *error = NULL;
	Pipe *pipe = NULL;
	gsize bytes_read = 0;
	GIOStatus status;

	/* Figure out which pipe we're using */
	if (source == priv->out.channel)
		pipe = &priv->out;
	else if (source == priv->err.channel)
		pipe = &priv->err;
	else
		g_assert_not_reached ();

	if (condition & G_IO_ERR) {
		_LOGW ("Unexpected vpnc pipe error");
		goto fail;
	}

	do {
		gsize consumed = 0;
		char buf[512];

		status = g_io_channel_read_chars (source,
		                                  buf,
		                                  sizeof (buf),
		                                  &bytes_read,
		                                  &error);
		if (status == G_IO_STATUS_ERROR) {
			if (error)
				_LOGW ("vpnc read error: %s", error->message);
			g_clear_error (&error);
		}

		if (bytes_read) {
			g_string_append_len (pipe->buf, buf, bytes_read);

			do {
				consumed = utils_handle_output (pipe->buf,
				                                priv->server_message,
				                                &priv->server_message_done,
				                                vpnc_prompt,
				                                plugin);
				if (consumed) {
					/* Log all output to the console */
					fprintf (pipe->logf, "%.*s", (int) consumed, pipe->buf->str);
					fflush (pipe->logf);

					/* If output was handled, clear the buffer */
					g_string_erase (pipe->buf, 0, consumed);
				}
			} while (consumed);
		}

		if (status == G_IO_STATUS_EOF)
			goto fail;
	} while (bytes_read);

	return TRUE;

fail:
	pipe->watch = 0;
	return FALSE;
}