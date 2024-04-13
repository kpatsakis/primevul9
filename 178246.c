static void enable_fallback(bool enable)
{
	GSList *list;

	for (list = server_list; list; list = list->next) {
		struct server_data *data = list->data;

		if (data->index != -1)
			continue;

		if (enable)
			DBG("Enabling fallback DNS server %s", data->server);
		else
			DBG("Disabling fallback DNS server %s", data->server);

		data->enabled = enable;
	}
}