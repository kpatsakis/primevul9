int __connman_dnsproxy_remove(int index, const char *domain,
							const char *server)
{
	DBG("index %d server %s", index, server);

	if (!server && !domain)
		return -EINVAL;

	if (!server) {
		remove_domain(index, domain);

		return 0;
	}

	if (g_str_equal(server, "127.0.0.1"))
		return -ENODEV;

	if (g_str_equal(server, "::1"))
		return -ENODEV;

	remove_server(index, domain, server, IPPROTO_UDP);
	remove_server(index, domain, server, IPPROTO_TCP);

	return 0;
}