int __connman_dnsproxy_init(void)
{
	int err, index;

	DBG("");

	listener_table = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							NULL, g_free);

	partial_tcp_req_table = g_hash_table_new_full(g_direct_hash,
							g_direct_equal,
							NULL,
							free_partial_reqs);

	index = connman_inet_ifindex("lo");
	err = __connman_dnsproxy_add_listener(index);
	if (err < 0)
		return err;

	err = connman_notifier_register(&dnsproxy_notifier);
	if (err < 0)
		goto destroy;

	return 0;

destroy:
	__connman_dnsproxy_remove_listener(index);
	g_hash_table_destroy(listener_table);
	g_hash_table_destroy(partial_tcp_req_table);

	return err;
}