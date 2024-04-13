static void destroy_udp_listener(struct listener_data *ifdata)
{
	DBG("index %d", ifdata->index);

	if (ifdata->udp4_listener_watch > 0)
		g_source_remove(ifdata->udp4_listener_watch);

	if (ifdata->udp6_listener_watch > 0)
		g_source_remove(ifdata->udp6_listener_watch);

	if (ifdata->udp4_listener_channel)
		g_io_channel_unref(ifdata->udp4_listener_channel);
	if (ifdata->udp6_listener_channel)
		g_io_channel_unref(ifdata->udp6_listener_channel);
}