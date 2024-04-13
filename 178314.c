static gboolean request_timeout(gpointer user_data)
{
	struct request_data *req = user_data;
	struct sockaddr *sa;
	int sk;

	if (!req)
		return FALSE;

	debug("id 0x%04x", req->srcid);

	request_list = g_slist_remove(request_list, req);

	if (req->protocol == IPPROTO_UDP) {
		sk = get_req_udp_socket(req);
		sa = &req->sa;
	} else if (req->protocol == IPPROTO_TCP) {
		sk = req->client_sk;
		sa = NULL;
	} else
		goto out;

	if (req->resplen > 0 && req->resp) {
		/*
		 * Here we have received at least one reply (probably telling
		 * "not found" result), so send that back to client instead
		 * of more fatal server failed error.
		 */
		if (sk >= 0)
			sendto(sk, req->resp, req->resplen, MSG_NOSIGNAL,
				sa, req->sa_len);

	} else if (req->request) {
		/*
		 * There was not reply from server at all.
		 */
		struct domain_hdr *hdr;

		hdr = (void *)(req->request + protocol_offset(req->protocol));
		hdr->id = req->srcid;

		if (sk >= 0)
			send_response(sk, req->request, req->request_len,
				sa, req->sa_len, req->protocol);
	}

	/*
	 * We cannot leave TCP client hanging so just kick it out
	 * if we get a request timeout from server.
	 */
	if (req->protocol == IPPROTO_TCP) {
		debug("client %d removed", req->client_sk);
		g_hash_table_remove(partial_tcp_req_table,
				GINT_TO_POINTER(req->client_sk));
	}

out:
	req->timeout = 0;
	destroy_request_data(req);

	return FALSE;
}