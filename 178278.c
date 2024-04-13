void __connman_dnsproxy_cleanup(void)
{
	DBG("");

	if (cache_timer) {
		g_source_remove(cache_timer);
		cache_timer = 0;
	}

	if (cache) {
		g_hash_table_destroy(cache);
		cache = NULL;
	}

	connman_notifier_unregister(&dnsproxy_notifier);

	g_hash_table_foreach(listener_table, remove_listener, NULL);

	g_hash_table_destroy(listener_table);

	g_hash_table_destroy(partial_tcp_req_table);

	if (ipv4_resolve)
		g_resolv_unref(ipv4_resolve);
	if (ipv6_resolve)
		g_resolv_unref(ipv6_resolve);
}