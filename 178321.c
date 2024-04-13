static void client_reset(struct tcp_partial_client_data *client)
{
	if (!client)
		return;

	if (client->channel) {
		debug("client %d closing",
			g_io_channel_unix_get_fd(client->channel));

		g_io_channel_unref(client->channel);
		client->channel = NULL;
	}

	if (client->watch > 0) {
		g_source_remove(client->watch);
		client->watch = 0;
	}

	if (client->timeout > 0) {
		g_source_remove(client->timeout);
		client->timeout = 0;
	}

	g_free(client->buf);
	client->buf = NULL;

	client->buf_end = 0;
}