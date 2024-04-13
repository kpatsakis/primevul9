static void flush_requests(struct server_data *server)
{
	GSList *list;

	list = request_list;
	while (list) {
		struct request_data *req = list->data;

		list = list->next;

		if (ns_resolv(server, req, req->request, req->name)) {
			/*
			 * A cached result was sent,
			 * so the request can be released
			 */
			request_list =
				g_slist_remove(request_list, req);
			destroy_request_data(req);
			continue;
		}

		if (req->timeout > 0)
			g_source_remove(req->timeout);

		req->timeout = g_timeout_add_seconds(5, request_timeout, req);
	}
}