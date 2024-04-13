void __connman_dnsproxy_remove_listener(int index)
{
	struct listener_data *ifdata;

	DBG("index %d", index);

	if (!listener_table)
		return;

	ifdata = g_hash_table_lookup(listener_table, GINT_TO_POINTER(index));
	if (!ifdata)
		return;

	destroy_listener(ifdata);

	g_hash_table_remove(listener_table, GINT_TO_POINTER(index));
}