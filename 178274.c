static gboolean udp_server_event(GIOChannel *channel, GIOCondition condition,
							gpointer user_data)
{
	unsigned char buf[4096];
	int sk, len;
	struct server_data *data = user_data;

	if (condition & (G_IO_NVAL | G_IO_ERR | G_IO_HUP)) {
		connman_error("Error with UDP server %s", data->server);
		server_destroy_socket(data);
		return FALSE;
	}

	sk = g_io_channel_unix_get_fd(channel);

	len = recv(sk, buf, sizeof(buf), 0);

	if (len >= 12)
		forward_dns_reply(buf, len, IPPROTO_UDP, data);

	return TRUE;
}