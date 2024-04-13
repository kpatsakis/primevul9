static gboolean tcp_server_event(GIOChannel *channel, GIOCondition condition,
							gpointer user_data)
{
	int sk;
	struct server_data *server = user_data;

	sk = g_io_channel_unix_get_fd(channel);
	if (sk == 0)
		return FALSE;

	if (condition & (G_IO_NVAL | G_IO_ERR | G_IO_HUP)) {
		GSList *list;
hangup:
		debug("TCP server channel closed, sk %d", sk);

		/*
		 * Discard any partial response which is buffered; better
		 * to get a proper response from a working server.
		 */
		g_free(server->incoming_reply);
		server->incoming_reply = NULL;

		list = request_list;
		while (list) {
			struct request_data *req = list->data;
			struct domain_hdr *hdr;
			list = list->next;

			if (req->protocol == IPPROTO_UDP)
				continue;

			if (!req->request)
				continue;

			/*
			 * If we're not waiting for any further response
			 * from another name server, then we send an error
			 * response to the client.
			 */
			if (req->numserv && --(req->numserv))
				continue;

			hdr = (void *) (req->request + 2);
			hdr->id = req->srcid;
			send_response(req->client_sk, req->request,
				req->request_len, NULL, 0, IPPROTO_TCP);

			request_list = g_slist_remove(request_list, req);
		}

		destroy_server(server);

		return FALSE;
	}

	if ((condition & G_IO_OUT) && !server->connected) {
		GSList *list;
		GList *domains;
		bool no_request_sent = true;
		struct server_data *udp_server;

		udp_server = find_server(server->index, server->server,
								IPPROTO_UDP);
		if (udp_server) {
			for (domains = udp_server->domains; domains;
						domains = domains->next) {
				char *dom = domains->data;

				debug("Adding domain %s to %s",
						dom, server->server);

				server->domains = g_list_append(server->domains,
								g_strdup(dom));
			}
		}

		server->connected = true;
		server_list = g_slist_append(server_list, server);

		if (server->timeout > 0) {
			g_source_remove(server->timeout);
			server->timeout = 0;
		}

		for (list = request_list; list; ) {
			struct request_data *req = list->data;
			int status;

			if (req->protocol == IPPROTO_UDP) {
				list = list->next;
				continue;
			}

			debug("Sending req %s over TCP", (char *)req->name);

			status = ns_resolv(server, req,
						req->request, req->name);
			if (status > 0) {
				/*
				 * A cached result was sent,
				 * so the request can be released
				 */
				list = list->next;
				request_list = g_slist_remove(request_list, req);
				destroy_request_data(req);
				continue;
			}

			if (status < 0) {
				list = list->next;
				continue;
			}

			no_request_sent = false;

			if (req->timeout > 0)
				g_source_remove(req->timeout);

			req->timeout = g_timeout_add_seconds(30,
						request_timeout, req);
			list = list->next;
		}

		if (no_request_sent) {
			destroy_server(server);
			return FALSE;
		}

	} else if (condition & G_IO_IN) {
		struct partial_reply *reply = server->incoming_reply;
		int bytes_recv;

		if (!reply) {
			unsigned char reply_len_buf[2];
			uint16_t reply_len;

			bytes_recv = recv(sk, reply_len_buf, 2, MSG_PEEK);
			if (!bytes_recv) {
				goto hangup;
			} else if (bytes_recv < 0) {
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					return TRUE;

				connman_error("DNS proxy error %s",
						strerror(errno));
				goto hangup;
			} else if (bytes_recv < 2)
				return TRUE;

			reply_len = reply_len_buf[1] | reply_len_buf[0] << 8;
			reply_len += 2;

			debug("TCP reply %d bytes from %d", reply_len, sk);

			reply = g_try_malloc(sizeof(*reply) + reply_len + 2);
			if (!reply)
				return TRUE;

			reply->len = reply_len;
			reply->received = 0;

			server->incoming_reply = reply;
		}

		while (reply->received < reply->len) {
			bytes_recv = recv(sk, reply->buf + reply->received,
					reply->len - reply->received, 0);
			if (!bytes_recv) {
				connman_error("DNS proxy TCP disconnect");
				break;
			} else if (bytes_recv < 0) {
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					return TRUE;

				connman_error("DNS proxy error %s",
						strerror(errno));
				break;
			}
			reply->received += bytes_recv;
		}

		forward_dns_reply(reply->buf, reply->received, IPPROTO_TCP,
					server);

		g_free(reply);
		server->incoming_reply = NULL;

		destroy_server(server);

		return FALSE;
	}

	return TRUE;
}