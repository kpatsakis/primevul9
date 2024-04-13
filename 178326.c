static bool resolv(struct request_data *req,
				gpointer request, gpointer name)
{
	GSList *list;

	for (list = server_list; list; list = list->next) {
		struct server_data *data = list->data;

		if (data->protocol == IPPROTO_TCP) {
			DBG("server %s ignored proto TCP", data->server);
			continue;
		}

		debug("server %s enabled %d", data->server, data->enabled);

		if (!data->enabled)
			continue;

		if (!data->channel && data->protocol == IPPROTO_UDP) {
			if (server_create_socket(data) < 0) {
				DBG("socket creation failed while resolving");
				continue;
			}
		}

		if (ns_resolv(data, req, request, name) > 0)
			return true;
	}

	return false;
}