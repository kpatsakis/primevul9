static void destroy_tcp_listener(struct listener_data *ifdata)
{
	DBG("index %d", ifdata->index);

	if (ifdata->tcp4_listener_watch > 0)
		g_source_remove(ifdata->tcp4_listener_watch);
	if (ifdata->tcp6_listener_watch > 0)
		g_source_remove(ifdata->tcp6_listener_watch);

	if (ifdata->tcp4_listener_channel)
		g_io_channel_unref(ifdata->tcp4_listener_channel);
	if (ifdata->tcp6_listener_channel)
		g_io_channel_unref(ifdata->tcp6_listener_channel);
}