static int ns_resolv(struct server_data *server, struct request_data *req,
				gpointer request, gpointer name)
{
	GList *list;
	int sk, err, type = 0;
	char *dot, *lookup = (char *) name;
	struct cache_entry *entry;

	entry = cache_check(request, &type, req->protocol);
	if (entry) {
		int ttl_left = 0;
		struct cache_data *data;

		debug("cache hit %s type %s", lookup, type == 1 ? "A" : "AAAA");
		if (type == 1)
			data = entry->ipv4;
		else
			data = entry->ipv6;

		if (data) {
			ttl_left = data->valid_until - time(NULL);
			entry->hits++;
		}

		if (data && req->protocol == IPPROTO_TCP) {
			send_cached_response(req->client_sk, data->data,
					data->data_len, NULL, 0, IPPROTO_TCP,
					req->srcid, data->answers, ttl_left);
			return 1;
		}

		if (data && req->protocol == IPPROTO_UDP) {
			int udp_sk = get_req_udp_socket(req);

			if (udp_sk < 0)
				return -EIO;

			send_cached_response(udp_sk, data->data,
				data->data_len, &req->sa, req->sa_len,
				IPPROTO_UDP, req->srcid, data->answers,
				ttl_left);
			return 1;
		}
	}

	sk = g_io_channel_unix_get_fd(server->channel);

	err = sendto(sk, request, req->request_len, MSG_NOSIGNAL,
			server->server_addr, server->server_addr_len);
	if (err < 0) {
		debug("Cannot send message to server %s sock %d "
			"protocol %d (%s/%d)",
			server->server, sk, server->protocol,
			strerror(errno), errno);
		return -EIO;
	}

	req->numserv++;

	/* If we have more than one dot, we don't add domains */
	dot = strchr(lookup, '.');
	if (dot && dot != lookup + strlen(lookup) - 1)
		return 0;

	if (server->domains && server->domains->data)
		req->append_domain = true;

	for (list = server->domains; list; list = list->next) {
		char *domain;
		unsigned char alt[1024];
		struct domain_hdr *hdr = (void *) &alt;
		int altlen, domlen, offset;

		domain = list->data;

		if (!domain)
			continue;

		offset = protocol_offset(server->protocol);
		if (offset < 0)
			return offset;

		domlen = strlen(domain) + 1;
		if (domlen < 5)
			return -EINVAL;

		alt[offset] = req->altid & 0xff;
		alt[offset + 1] = req->altid >> 8;

		memcpy(alt + offset + 2, request + offset + 2, 10);
		hdr->qdcount = htons(1);

		altlen = append_query(alt + offset + 12, sizeof(alt) - 12,
					name, domain);
		if (altlen < 0)
			return -EINVAL;

		altlen += 12;

		memcpy(alt + offset + altlen,
			request + offset + altlen - domlen,
				req->request_len - altlen - offset + domlen);

		if (server->protocol == IPPROTO_TCP) {
			int req_len = req->request_len + domlen - 2;

			alt[0] = (req_len >> 8) & 0xff;
			alt[1] = req_len & 0xff;
		}

		debug("req %p dstid 0x%04x altid 0x%04x", req, req->dstid,
				req->altid);

		err = send(sk, alt, req->request_len + domlen, MSG_NOSIGNAL);
		if (err < 0)
			return -EIO;

		req->numserv++;
	}

	return 0;
}