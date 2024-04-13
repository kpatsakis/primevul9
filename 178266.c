static bool tcp_listener_event(GIOChannel *channel, GIOCondition condition,
				struct listener_data *ifdata, int family,
				guint *listener_watch)
{
	int sk, client_sk, len;
	unsigned int msg_len;
	struct tcp_partial_client_data *client;
	struct sockaddr_in6 client_addr6;
	socklen_t client_addr6_len = sizeof(client_addr6);
	struct sockaddr_in client_addr4;
	socklen_t client_addr4_len = sizeof(client_addr4);
	void *client_addr;
	socklen_t *client_addr_len;
	struct timeval tv;
	fd_set readfds;

	debug("condition 0x%02x channel %p ifdata %p family %d",
		condition, channel, ifdata, family);

	if (condition & (G_IO_NVAL | G_IO_ERR | G_IO_HUP)) {
		if (*listener_watch > 0)
			g_source_remove(*listener_watch);
		*listener_watch = 0;

		connman_error("Error with TCP listener channel");

		return false;
	}

	sk = g_io_channel_unix_get_fd(channel);

	if (family == AF_INET) {
		client_addr = &client_addr4;
		client_addr_len = &client_addr4_len;
	} else {
		client_addr = &client_addr6;
		client_addr_len = &client_addr6_len;
	}

	tv.tv_sec = tv.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_SET(sk, &readfds);

	select(sk + 1, &readfds, NULL, NULL, &tv);
	if (FD_ISSET(sk, &readfds)) {
		client_sk = accept(sk, client_addr, client_addr_len);
		debug("client %d accepted", client_sk);
	} else {
		debug("No data to read from master %d, waiting.", sk);
		return true;
	}

	if (client_sk < 0) {
		connman_error("Accept failure on TCP listener");
		*listener_watch = 0;
		return false;
	}

	fcntl(client_sk, F_SETFL, O_NONBLOCK);

	client = g_hash_table_lookup(partial_tcp_req_table,
					GINT_TO_POINTER(client_sk));
	if (!client) {
		client = g_try_new0(struct tcp_partial_client_data, 1);
		if (!client) {
			close(client_sk);
			return false;
		}

		g_hash_table_insert(partial_tcp_req_table,
					GINT_TO_POINTER(client_sk),
					client);

		client->channel = g_io_channel_unix_new(client_sk);
		g_io_channel_set_close_on_unref(client->channel, TRUE);

		client->watch = g_io_add_watch(client->channel,
						G_IO_IN, tcp_client_event,
						(gpointer)client);

		client->ifdata = ifdata;

		debug("client %d created %p", client_sk, client);
	} else {
		debug("client %d already exists %p", client_sk, client);
	}

	if (!client->buf) {
		client->buf = g_try_malloc(TCP_MAX_BUF_LEN);
		if (!client->buf)
			return false;
	}
	memset(client->buf, 0, TCP_MAX_BUF_LEN);
	client->buf_end = 0;
	client->family = family;

	if (client->timeout == 0)
		client->timeout = g_timeout_add_seconds(2, client_timeout,
							client);

	/*
	 * Check how much data there is. If all is there, then we can
	 * proceed normally, otherwise read the bits until everything
	 * is received or timeout occurs.
	 */
	len = recv(client_sk, client->buf, TCP_MAX_BUF_LEN, 0);
	if (len < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			debug("client %d no data to read, waiting", client_sk);
			return true;
		}

		debug("client %d cannot read errno %d/%s", client_sk, -errno,
			strerror(errno));
		g_hash_table_remove(partial_tcp_req_table,
					GINT_TO_POINTER(client_sk));
		return true;
	}

	if (len < 2) {
		debug("client %d not enough data to read, waiting", client_sk);
		client->buf_end += len;
		return true;
	}

	msg_len = get_msg_len(client->buf);
	if (msg_len > TCP_MAX_BUF_LEN) {
		debug("client %d invalid message length %u ignoring packet",
			client_sk, msg_len);
		g_hash_table_remove(partial_tcp_req_table,
					GINT_TO_POINTER(client_sk));
		return true;
	}

	/*
	 * The packet length bytes do not contain the total message length,
	 * that is the reason to -2 below.
	 */
	if (msg_len != (unsigned int)(len - 2)) {
		debug("client %d sent %d bytes but expecting %u pending %d",
			client_sk, len, msg_len + 2, msg_len + 2 - len);

		client->buf_end += len;
		return true;
	}

	return read_tcp_data(client, client_addr, *client_addr_len, len);
}