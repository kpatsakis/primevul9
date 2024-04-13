static void remove_server(int index, const char *domain,
			const char *server, int protocol)
{
	struct server_data *data;
	GSList *list;

	data = find_server(index, server, protocol);
	if (!data)
		return;

	destroy_server(data);

	for (list = server_list; list; list = list->next) {
		struct server_data *data = list->data;

		if (data->index != -1 && data->enabled == true)
			return;
	}

	enable_fallback(true);
}