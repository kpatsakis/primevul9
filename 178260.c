static void dnsproxy_default_changed(struct connman_service *service)
{
	bool server_enabled = false;
	GSList *list;
	int index;

	DBG("service %p", service);

	/* DNS has changed, invalidate the cache */
	cache_invalidate();

	if (!service) {
		/* When no services are active, then disable DNS proxying */
		dnsproxy_offline_mode(true);
		return;
	}

	index = __connman_service_get_index(service);
	if (index < 0)
		return;

	for (list = server_list; list; list = list->next) {
		struct server_data *data = list->data;

		if (data->index == index) {
			DBG("Enabling DNS server %s", data->server);
			data->enabled = true;
			server_enabled = true;
		} else {
			DBG("Disabling DNS server %s", data->server);
			data->enabled = false;
		}
	}

	if (!server_enabled)
		enable_fallback(true);

	cache_refresh();
}