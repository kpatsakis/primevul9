static void destroy_server(struct server_data *server)
{
	debug("index %d server %s sock %d", server->index, server->server,
			server->channel ?
			g_io_channel_unix_get_fd(server->channel): -1);

	server_list = g_slist_remove(server_list, server);
	server_destroy_socket(server);

	if (server->protocol == IPPROTO_UDP && server->enabled)
		debug("Removing DNS server %s", server->server);

	g_free(server->server);
	g_list_free_full(server->domains, g_free);
	g_free(server->server_addr);

	/*
	 * We do not remove cache right away but delay it few seconds.
	 * The idea is that when IPv6 DNS server is added via RDNSS, it has a
	 * lifetime. When the lifetime expires we decrease the refcount so it
	 * is possible that the cache is then removed. Because a new DNS server
	 * is usually created almost immediately we would then loose the cache
	 * without any good reason. The small delay allows the new RDNSS to
	 * create a new DNS server instance and the refcount does not go to 0.
	 */
	if (cache && !cache_timer)
		cache_timer = g_timeout_add_seconds(3, try_remove_cache, NULL);

	g_free(server);
}