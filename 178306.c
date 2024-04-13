int __connman_dnsproxy_append(int index, const char *domain,
							const char *server)
{
	struct server_data *data;

	DBG("index %d server %s", index, server);

	if (!server && !domain)
		return -EINVAL;

	if (!server) {
		append_domain(index, domain);

		return 0;
	}

	if (g_str_equal(server, "127.0.0.1"))
		return -ENODEV;

	if (g_str_equal(server, "::1"))
		return -ENODEV;

	data = find_server(index, server, IPPROTO_UDP);
	if (data) {
		append_domain(index, domain);
		return 0;
	}

	data = create_server(index, domain, server, IPPROTO_UDP);
	if (!data)
		return -EIO;

	flush_requests(data);

	return 0;
}