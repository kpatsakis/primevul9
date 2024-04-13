static gboolean tcp4_listener_event(GIOChannel *channel, GIOCondition condition,
				gpointer user_data)
{
	struct listener_data *ifdata = user_data;

	return tcp_listener_event(channel, condition, ifdata, AF_INET,
				&ifdata->tcp4_listener_watch);
}