static int get_req_udp_socket(struct request_data *req)
{
	GIOChannel *channel;

	if (req->family == AF_INET)
		channel = req->ifdata->udp4_listener_channel;
	else
		channel = req->ifdata->udp6_listener_channel;

	if (!channel)
		return -1;

	return g_io_channel_unix_get_fd(channel);
}