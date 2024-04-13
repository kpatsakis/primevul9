static void destroy_listener(struct listener_data *ifdata)
{
	int index;
	GSList *list;

	index = connman_inet_ifindex("lo");
	if (ifdata->index == index) {
		__connman_resolvfile_remove(index, NULL, "127.0.0.1");
		__connman_resolvfile_remove(index, NULL, "::1");
	}

	for (list = request_list; list; list = list->next) {
		struct request_data *req = list->data;

		debug("Dropping request (id 0x%04x -> 0x%04x)",
						req->srcid, req->dstid);
		destroy_request_data(req);
		list->data = NULL;
	}

	g_slist_free(request_list);
	request_list = NULL;

	destroy_tcp_listener(ifdata);
	destroy_udp_listener(ifdata);
}