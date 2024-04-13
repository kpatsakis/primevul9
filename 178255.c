static void server_destroy_socket(struct server_data *data)
{
	debug("index %d server %s proto %d", data->index,
					data->server, data->protocol);

	if (data->watch > 0) {
		g_source_remove(data->watch);
		data->watch = 0;
	}

	if (data->timeout > 0) {
		g_source_remove(data->timeout);
		data->timeout = 0;
	}

	if (data->channel) {
		g_io_channel_shutdown(data->channel, TRUE, NULL);
		g_io_channel_unref(data->channel);
		data->channel = NULL;
	}

	g_free(data->incoming_reply);
	data->incoming_reply = NULL;
}