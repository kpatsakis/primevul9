static bool read_tcp_data(struct tcp_partial_client_data *client,
				void *client_addr, socklen_t client_addr_len,
				int read_len)
{
	char query[TCP_MAX_BUF_LEN];
	struct request_data *req;
	int client_sk, err;
	unsigned int msg_len;
	GSList *list;
	bool waiting_for_connect = false;
	int qtype = 0;
	struct cache_entry *entry;

	client_sk = g_io_channel_unix_get_fd(client->channel);

	if (read_len == 0) {
		debug("client %d closed, pending %d bytes",
			client_sk, client->buf_end);
		g_hash_table_remove(partial_tcp_req_table,
					GINT_TO_POINTER(client_sk));
		return false;
	}

	debug("client %d received %d bytes", client_sk, read_len);

	client->buf_end += read_len;

	if (client->buf_end < 2)
		return true;

	msg_len = get_msg_len(client->buf);
	if (msg_len > TCP_MAX_BUF_LEN) {
		debug("client %d sent too much data %d", client_sk, msg_len);
		g_hash_table_remove(partial_tcp_req_table,
					GINT_TO_POINTER(client_sk));
		return false;
	}

read_another:
	debug("client %d msg len %d end %d past end %d", client_sk, msg_len,
		client->buf_end, client->buf_end - (msg_len + 2));

	if (client->buf_end < (msg_len + 2)) {
		debug("client %d still missing %d bytes",
			client_sk,
			msg_len + 2 - client->buf_end);
		return true;
	}

	debug("client %d all data %d received", client_sk, msg_len);

	err = parse_request(client->buf + 2, msg_len,
			query, sizeof(query));
	if (err < 0 || (g_slist_length(server_list) == 0)) {
		send_response(client_sk, client->buf, msg_len + 2,
			NULL, 0, IPPROTO_TCP);
		return true;
	}

	req = g_try_new0(struct request_data, 1);
	if (!req)
		return true;

	memcpy(&req->sa, client_addr, client_addr_len);
	req->sa_len = client_addr_len;
	req->client_sk = client_sk;
	req->protocol = IPPROTO_TCP;
	req->family = client->family;

	req->srcid = client->buf[2] | (client->buf[3] << 8);
	req->dstid = get_id();
	req->altid = get_id();
	req->request_len = msg_len + 2;

	client->buf[2] = req->dstid & 0xff;
	client->buf[3] = req->dstid >> 8;

	req->numserv = 0;
	req->ifdata = client->ifdata;
	req->append_domain = false;

	/*
	 * Check if the answer is found in the cache before
	 * creating sockets to the server.
	 */
	entry = cache_check(client->buf, &qtype, IPPROTO_TCP);
	if (entry) {
		int ttl_left = 0;
		struct cache_data *data;

		debug("cache hit %s type %s", query, qtype == 1 ? "A" : "AAAA");
		if (qtype == 1)
			data = entry->ipv4;
		else
			data = entry->ipv6;

		if (data) {
			ttl_left = data->valid_until - time(NULL);
			entry->hits++;

			send_cached_response(client_sk, data->data,
					data->data_len, NULL, 0, IPPROTO_TCP,
					req->srcid, data->answers, ttl_left);

			g_free(req);
			goto out;
		} else
			debug("data missing, ignoring cache for this query");
	}

	for (list = server_list; list; list = list->next) {
		struct server_data *data = list->data;

		if (data->protocol != IPPROTO_UDP || !data->enabled)
			continue;

		if (!create_server(data->index, NULL, data->server,
					IPPROTO_TCP))
			continue;

		waiting_for_connect = true;
	}

	if (!waiting_for_connect) {
		/* No server is waiting for connect */
		send_response(client_sk, client->buf,
			req->request_len, NULL, 0, IPPROTO_TCP);
		g_free(req);
		return true;
	}

	/*
	 * The server is not connected yet.
	 * Copy the relevant buffers.
	 * The request will actually be sent once we're
	 * properly connected over TCP to the nameserver.
	 */
	req->request = g_try_malloc0(req->request_len);
	if (!req->request) {
		send_response(client_sk, client->buf,
			req->request_len, NULL, 0, IPPROTO_TCP);
		g_free(req);
		goto out;
	}
	memcpy(req->request, client->buf, req->request_len);

	req->name = g_try_malloc0(sizeof(query));
	if (!req->name) {
		send_response(client_sk, client->buf,
			req->request_len, NULL, 0, IPPROTO_TCP);
		g_free(req->request);
		g_free(req);
		goto out;
	}
	memcpy(req->name, query, sizeof(query));

	req->timeout = g_timeout_add_seconds(30, request_timeout, req);

	request_list = g_slist_append(request_list, req);

out:
	if (client->buf_end > (msg_len + 2)) {
		debug("client %d buf %p -> %p end %d len %d new %d",
			client_sk,
			client->buf + msg_len + 2,
			client->buf, client->buf_end,
			TCP_MAX_BUF_LEN - client->buf_end,
			client->buf_end - (msg_len + 2));
		memmove(client->buf, client->buf + msg_len + 2,
			TCP_MAX_BUF_LEN - client->buf_end);
		client->buf_end = client->buf_end - (msg_len + 2);

		/*
		 * If we have a full message waiting, just read it
		 * immediately.
		 */
		msg_len = get_msg_len(client->buf);
		if ((msg_len + 2) == client->buf_end) {
			debug("client %d reading another %d bytes", client_sk,
								msg_len + 2);
			goto read_another;
		}
	} else {
		debug("client %d clearing reading buffer", client_sk);

		client->buf_end = 0;
		memset(client->buf, 0, TCP_MAX_BUF_LEN);

		/*
		 * We received all the packets from client so we must also
		 * remove the timeout handler here otherwise we might get
		 * timeout while waiting the results from server.
		 */
		g_source_remove(client->timeout);
		client->timeout = 0;
	}

	return true;
}