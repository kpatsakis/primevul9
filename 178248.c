static int create_dns_listener(int protocol, struct listener_data *ifdata)
{
	int ret = 0;

	if (protocol == IPPROTO_TCP) {
		ifdata->tcp4_listener_channel = get_listener(AF_INET, protocol,
							ifdata->index);
		if (ifdata->tcp4_listener_channel)
			ifdata->tcp4_listener_watch =
				g_io_add_watch(ifdata->tcp4_listener_channel,
					G_IO_IN, tcp4_listener_event,
					(gpointer)ifdata);
		else
			ret |= TCP_IPv4_FAILED;

		ifdata->tcp6_listener_channel = get_listener(AF_INET6, protocol,
							ifdata->index);
		if (ifdata->tcp6_listener_channel)
			ifdata->tcp6_listener_watch =
				g_io_add_watch(ifdata->tcp6_listener_channel,
					G_IO_IN, tcp6_listener_event,
					(gpointer)ifdata);
		else
			ret |= TCP_IPv6_FAILED;
	} else {
		ifdata->udp4_listener_channel = get_listener(AF_INET, protocol,
							ifdata->index);
		if (ifdata->udp4_listener_channel)
			ifdata->udp4_listener_watch =
				g_io_add_watch(ifdata->udp4_listener_channel,
					G_IO_IN, udp4_listener_event,
					(gpointer)ifdata);
		else
			ret |= UDP_IPv4_FAILED;

		ifdata->udp6_listener_channel = get_listener(AF_INET6, protocol,
							ifdata->index);
		if (ifdata->udp6_listener_channel)
			ifdata->udp6_listener_watch =
				g_io_add_watch(ifdata->udp6_listener_channel,
					G_IO_IN, udp6_listener_event,
					(gpointer)ifdata);
		else
			ret |= UDP_IPv6_FAILED;
	}

	return ret;
}