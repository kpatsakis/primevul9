static gboolean udp4_listener_event(GIOChannel *channel, GIOCondition condition,
				gpointer user_data)
{
	struct listener_data *ifdata = user_data;

	return udp_listener_event(channel, condition, ifdata, AF_INET,
				&ifdata->udp4_listener_watch);
}