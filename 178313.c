static bool udp_listener_event(GIOChannel *channel, GIOCondition condition,
				struct listener_data *ifdata, int family,
				guint *listener_watch)
{
	unsigned char buf[768];
	char query[512];
	struct request_data *req;
	struct sockaddr_in6 client_addr6;
	socklen_t client_addr6_len = sizeof(client_addr6);
	struct sockaddr_in client_addr4;
	socklen_t client_addr4_len = sizeof(client_addr4);
	void *client_addr;
	socklen_t *client_addr_len;
	int sk, err, len;

	if (condition & (G_IO_NVAL | G_IO_ERR | G_IO_HUP)) {
		connman_error("Error with UDP listener channel");
		*listener_watch = 0;
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

	memset(client_addr, 0, *client_addr_len);
	len = recvfrom(sk, buf, sizeof(buf), 0, client_addr, client_addr_len);
	if (len < 2)
		return true;

	debug("Received %d bytes (id 0x%04x)", len, buf[0] | buf[1] << 8);

	err = parse_request(buf, len, query, sizeof(query));
	if (err < 0 || (g_slist_length(server_list) == 0)) {
		send_response(sk, buf, len, client_addr,
				*client_addr_len, IPPROTO_UDP);
		return true;
	}

	req = g_try_new0(struct request_data, 1);
	if (!req)
		return true;

	memcpy(&req->sa, client_addr, *client_addr_len);
	req->sa_len = *client_addr_len;
	req->client_sk = 0;
	req->protocol = IPPROTO_UDP;
	req->family = family;

	req->srcid = buf[0] | (buf[1] << 8);
	req->dstid = get_id();
	req->altid = get_id();
	req->request_len = len;

	buf[0] = req->dstid & 0xff;
	buf[1] = req->dstid >> 8;

	req->numserv = 0;
	req->ifdata = ifdata;
	req->append_domain = false;

	if (resolv(req, buf, query)) {
		/* a cached result was sent, so the request can be released */
	        g_free(req);
		return true;
	}

	req->name = g_strdup(query);
	req->request = g_malloc(len);
	memcpy(req->request, buf, len);
	req->timeout = g_timeout_add_seconds(5, request_timeout, req);
	request_list = g_slist_append(request_list, req);

	return true;
}