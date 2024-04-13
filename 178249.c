int __connman_dnsproxy_add_listener(int index)
{
	struct listener_data *ifdata;
	int err;

	DBG("index %d", index);

	if (index < 0)
		return -EINVAL;

	if (!listener_table)
		return -ENOENT;

	if (g_hash_table_lookup(listener_table, GINT_TO_POINTER(index)))
		return 0;

	ifdata = g_try_new0(struct listener_data, 1);
	if (!ifdata)
		return -ENOMEM;

	ifdata->index = index;
	ifdata->udp4_listener_channel = NULL;
	ifdata->udp4_listener_watch = 0;
	ifdata->tcp4_listener_channel = NULL;
	ifdata->tcp4_listener_watch = 0;
	ifdata->udp6_listener_channel = NULL;
	ifdata->udp6_listener_watch = 0;
	ifdata->tcp6_listener_channel = NULL;
	ifdata->tcp6_listener_watch = 0;

	err = create_listener(ifdata);
	if (err < 0) {
		connman_error("Couldn't create listener for index %d err %d",
				index, err);
		g_free(ifdata);
		return err;
	}
	g_hash_table_insert(listener_table, GINT_TO_POINTER(ifdata->index),
			ifdata);
	return 0;
}