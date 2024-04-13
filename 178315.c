static int server_create_socket(struct server_data *data)
{
	int sk, err;
	char *interface;

	debug("index %d server %s proto %d", data->index,
					data->server, data->protocol);

	sk = socket(data->server_addr->sa_family,
		data->protocol == IPPROTO_TCP ? SOCK_STREAM : SOCK_DGRAM,
		data->protocol);
	if (sk < 0) {
		err = errno;
		connman_error("Failed to create server %s socket",
							data->server);
		server_destroy_socket(data);
		return -err;
	}

	debug("sk %d", sk);

	interface = connman_inet_ifname(data->index);
	if (interface) {
		if (setsockopt(sk, SOL_SOCKET, SO_BINDTODEVICE,
					interface,
					strlen(interface) + 1) < 0) {
			err = errno;
			connman_error("Failed to bind server %s "
						"to interface %s",
						data->server, interface);
			close(sk);
			server_destroy_socket(data);
			g_free(interface);
			return -err;
		}
		g_free(interface);
	}

	data->channel = g_io_channel_unix_new(sk);
	if (!data->channel) {
		connman_error("Failed to create server %s channel",
							data->server);
		close(sk);
		server_destroy_socket(data);
		return -ENOMEM;
	}

	g_io_channel_set_close_on_unref(data->channel, TRUE);

	if (data->protocol == IPPROTO_TCP) {
		g_io_channel_set_flags(data->channel, G_IO_FLAG_NONBLOCK, NULL);
		data->watch = g_io_add_watch(data->channel,
			G_IO_OUT | G_IO_IN | G_IO_HUP | G_IO_NVAL | G_IO_ERR,
						tcp_server_event, data);
		data->timeout = g_timeout_add_seconds(30, tcp_idle_timeout,
								data);
	} else
		data->watch = g_io_add_watch(data->channel,
			G_IO_IN | G_IO_NVAL | G_IO_ERR | G_IO_HUP,
						udp_server_event, data);

	if (connect(sk, data->server_addr, data->server_addr_len) < 0) {
		err = errno;

		if ((data->protocol == IPPROTO_TCP && errno != EINPROGRESS) ||
				data->protocol == IPPROTO_UDP) {

			connman_error("Failed to connect to server %s",
								data->server);
			server_destroy_socket(data);
			return -err;
		}
	}

	create_cache();

	return 0;
}