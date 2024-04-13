static gboolean tcp_client_event(GIOChannel *channel, GIOCondition condition,
				gpointer user_data)
{
	struct tcp_partial_client_data *client = user_data;
	struct sockaddr_in6 client_addr6;
	socklen_t client_addr6_len = sizeof(client_addr6);
	struct sockaddr_in client_addr4;
	socklen_t client_addr4_len = sizeof(client_addr4);
	void *client_addr;
	socklen_t *client_addr_len;
	int len, client_sk;

	client_sk = g_io_channel_unix_get_fd(channel);

	if (condition & (G_IO_NVAL | G_IO_ERR | G_IO_HUP)) {
		g_hash_table_remove(partial_tcp_req_table,
					GINT_TO_POINTER(client_sk));

		connman_error("Error with TCP client %d channel", client_sk);
		return FALSE;
	}

	switch (client->family) {
	case AF_INET:
		client_addr = &client_addr4;
		client_addr_len = &client_addr4_len;
		break;
	case AF_INET6:
		client_addr = &client_addr6;
		client_addr_len = &client_addr6_len;
		break;
	default:
		g_hash_table_remove(partial_tcp_req_table,
					GINT_TO_POINTER(client_sk));
		connman_error("client %p corrupted", client);
		return FALSE;
	}

	len = recvfrom(client_sk, client->buf + client->buf_end,
			TCP_MAX_BUF_LEN - client->buf_end, 0,
			client_addr, client_addr_len);
	if (len < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return TRUE;

		debug("client %d cannot read errno %d/%s", client_sk, -errno,
			strerror(errno));
		g_hash_table_remove(partial_tcp_req_table,
					GINT_TO_POINTER(client_sk));
		return FALSE;
	}

	return read_tcp_data(client, client_addr, *client_addr_len, len);
}