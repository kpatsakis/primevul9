static void update_domain(int index, const char *domain, bool append)
{
	GSList *list;

	DBG("index %d domain %s", index, domain);

	if (!domain)
		return;

	for (list = server_list; list; list = list->next) {
		struct server_data *data = list->data;
		GList *dom_list;
		char *dom;
		bool dom_found = false;

		if (data->index < 0)
			continue;

		if (data->index != index)
			continue;

		for (dom_list = data->domains; dom_list;
				dom_list = dom_list->next) {
			dom = dom_list->data;

			if (g_str_equal(dom, domain)) {
				dom_found = true;
				break;
			}
		}

		if (!dom_found && append) {
			data->domains =
				g_list_append(data->domains, g_strdup(domain));
		} else if (dom_found && !append) {
			data->domains =
				g_list_remove(data->domains, dom);
			g_free(dom);
		}
	}
}