static void dnsproxy_service_state_changed(struct connman_service *service,
			enum connman_service_state state)
{
	GSList *list;
	int index;

	switch (state) {
	case CONNMAN_SERVICE_STATE_DISCONNECT:
	case CONNMAN_SERVICE_STATE_IDLE:
		break;
	case CONNMAN_SERVICE_STATE_ASSOCIATION:
	case CONNMAN_SERVICE_STATE_CONFIGURATION:
	case CONNMAN_SERVICE_STATE_FAILURE:
	case CONNMAN_SERVICE_STATE_ONLINE:
	case CONNMAN_SERVICE_STATE_READY:
	case CONNMAN_SERVICE_STATE_UNKNOWN:
		return;
	}

	index = __connman_service_get_index(service);
	list = server_list;

	while (list) {
		struct server_data *data = list->data;

		/* Get next before the list is changed by destroy_server() */
		list = list->next;

		if (data->index == index) {
			DBG("removing server data of index %d", index);
			destroy_server(data);
		}
	}
}