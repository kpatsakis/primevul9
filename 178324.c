static void dnsproxy_offline_mode(bool enabled)
{
	GSList *list;

	DBG("enabled %d", enabled);

	for (list = server_list; list; list = list->next) {
		struct server_data *data = list->data;

		if (!enabled) {
			DBG("Enabling DNS server %s", data->server);
			data->enabled = true;
			cache_invalidate();
			cache_refresh();
		} else {
			DBG("Disabling DNS server %s", data->server);
			data->enabled = false;
			cache_invalidate();
		}
	}
}