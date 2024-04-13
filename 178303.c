static struct server_data *create_server(int index,
					const char *domain, const char *server,
					int protocol)
{
	struct server_data *data;
	struct addrinfo hints, *rp;
	int ret;

	DBG("index %d server %s", index, server);

	data = g_try_new0(struct server_data, 1);
	if (!data) {
		connman_error("Failed to allocate server %s data", server);
		return NULL;
	}

	data->index = index;
	if (domain)
		data->domains = g_list_append(data->domains, g_strdup(domain));
	data->server = g_strdup(server);
	data->protocol = protocol;

	memset(&hints, 0, sizeof(hints));

	switch (protocol) {
	case IPPROTO_UDP:
		hints.ai_socktype = SOCK_DGRAM;
		break;

	case IPPROTO_TCP:
		hints.ai_socktype = SOCK_STREAM;
		break;

	default:
		destroy_server(data);
		return NULL;
	}
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_NUMERICSERV | AI_NUMERICHOST;

	ret = getaddrinfo(data->server, "53", &hints, &rp);
	if (ret) {
		connman_error("Failed to parse server %s address: %s\n",
			      data->server, gai_strerror(ret));
		destroy_server(data);
		return NULL;
	}

	/* Do not blindly copy this code elsewhere; it doesn't loop over the
	   results using ->ai_next as it should. That's OK in *this* case
	   because it was a numeric lookup; we *know* there's only one. */

	data->server_addr_len = rp->ai_addrlen;

	switch (rp->ai_family) {
	case AF_INET:
		data->server_addr = (struct sockaddr *)
					g_try_new0(struct sockaddr_in, 1);
		break;
	case AF_INET6:
		data->server_addr = (struct sockaddr *)
					g_try_new0(struct sockaddr_in6, 1);
		break;
	default:
		connman_error("Wrong address family %d", rp->ai_family);
		break;
	}
	if (!data->server_addr) {
		freeaddrinfo(rp);
		destroy_server(data);
		return NULL;
	}
	memcpy(data->server_addr, rp->ai_addr, rp->ai_addrlen);
	freeaddrinfo(rp);

	if (server_create_socket(data) != 0) {
		destroy_server(data);
		return NULL;
	}

	if (protocol == IPPROTO_UDP) {
		if (__connman_service_index_is_default(data->index) ||
				__connman_service_index_is_split_routing(
								data->index)) {
			data->enabled = true;
			DBG("Adding DNS server %s", data->server);

			enable_fallback(false);
		}

		server_list = g_slist_append(server_list, data);
	}

	return data;
}