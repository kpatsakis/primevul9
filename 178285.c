static struct server_data *find_server(int index,
					const char *server,
						int protocol)
{
	GSList *list;

	debug("index %d server %s proto %d", index, server, protocol);

	for (list = server_list; list; list = list->next) {
		struct server_data *data = list->data;

		if (index < 0 && data->index < 0 &&
				g_str_equal(data->server, server) &&
				data->protocol == protocol)
			return data;

		if (index < 0 ||
				data->index < 0 || !data->server)
			continue;

		if (data->index == index &&
				g_str_equal(data->server, server) &&
				data->protocol == protocol)
			return data;
	}

	return NULL;
}