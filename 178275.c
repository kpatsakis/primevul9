static gboolean tcp6_listener_event(GIOChannel *channel, GIOCondition condition,
				gpointer user_data)
{
	struct listener_data *ifdata = user_data;

	return tcp_listener_event(channel, condition, user_data, AF_INET6,
				&ifdata->tcp6_listener_watch);
}