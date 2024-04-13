static int forward_dns_reply(unsigned char *reply, int reply_len, int protocol,
				struct server_data *data)
{
	struct domain_hdr *hdr;
	struct request_data *req;
	int dns_id, sk, err, offset = protocol_offset(protocol);

	if (offset < 0)
		return offset;

	hdr = (void *)(reply + offset);
	dns_id = reply[offset] | reply[offset + 1] << 8;

	debug("Received %d bytes (id 0x%04x)", reply_len, dns_id);

	req = find_request(dns_id);
	if (!req)
		return -EINVAL;

	debug("req %p dstid 0x%04x altid 0x%04x rcode %d",
			req, req->dstid, req->altid, hdr->rcode);

	reply[offset] = req->srcid & 0xff;
	reply[offset + 1] = req->srcid >> 8;

	req->numresp++;

	if (hdr->rcode == ns_r_noerror || !req->resp) {
		unsigned char *new_reply = NULL;

		/*
		 * If the domain name was append
		 * remove it before forwarding the reply.
		 * If there were more than one question, then this
		 * domain name ripping can be hairy so avoid that
		 * and bail out in that that case.
		 *
		 * The reason we are doing this magic is that if the
		 * user's DNS client tries to resolv hostname without
		 * domain part, it also expects to get the result without
		 * a domain name part.
		 */
		if (req->append_domain && ntohs(hdr->qdcount) == 1) {
			uint16_t domain_len = 0;
			uint16_t header_len;
			uint16_t dns_type, dns_class;
			uint8_t host_len, dns_type_pos;
			char uncompressed[NS_MAXDNAME], *uptr;
			char *ptr, *eom = (char *)reply + reply_len;

			/*
			 * ptr points to the first char of the hostname.
			 * ->hostname.domain.net
			 */
			header_len = offset + sizeof(struct domain_hdr);
			ptr = (char *)reply + header_len;

			host_len = *ptr;
			if (host_len > 0)
				domain_len = strnlen(ptr + 1 + host_len,
						reply_len - header_len);

			/*
			 * If the query type is anything other than A or AAAA,
			 * then bail out and pass the message as is.
			 * We only want to deal with IPv4 or IPv6 addresses.
			 */
			dns_type_pos = host_len + 1 + domain_len + 1;

			dns_type = ptr[dns_type_pos] << 8 |
							ptr[dns_type_pos + 1];
			dns_class = ptr[dns_type_pos + 2] << 8 |
							ptr[dns_type_pos + 3];
			if (dns_type != ns_t_a && dns_type != ns_t_aaaa &&
					dns_class != ns_c_in) {
				debug("Pass msg dns type %d class %d",
					dns_type, dns_class);
				goto pass;
			}

			/*
			 * Remove the domain name and replace it by the end
			 * of reply. Check if the domain is really there
			 * before trying to copy the data. We also need to
			 * uncompress the answers if necessary.
			 * The domain_len can be 0 because if the original
			 * query did not contain a domain name, then we are
			 * sending two packets, first without the domain name
			 * and the second packet with domain name.
			 * The append_domain is set to true even if we sent
			 * the first packet without domain name. In this
			 * case we end up in this branch.
			 */
			if (domain_len > 0) {
				int len = host_len + 1;
				int new_len, fixed_len;
				char *answers;

				/*
				 * First copy host (without domain name) into
				 * tmp buffer.
				 */
				uptr = &uncompressed[0];
				memcpy(uptr, ptr, len);

				uptr[len] = '\0'; /* host termination */
				uptr += len + 1;

				/*
				 * Copy type and class fields of the question.
				 */
				ptr += len + domain_len + 1;
				memcpy(uptr, ptr, NS_QFIXEDSZ);

				/*
				 * ptr points to answers after this
				 */
				ptr += NS_QFIXEDSZ;
				uptr += NS_QFIXEDSZ;
				answers = uptr;
				fixed_len = answers - uncompressed;

				/*
				 * We then uncompress the result to buffer
				 * so that we can rip off the domain name
				 * part from the question. First answers,
				 * then name server (authority) information,
				 * and finally additional record info.
				 */

				ptr = uncompress(ntohs(hdr->ancount),
						(char *)reply + offset, eom,
						ptr, uncompressed, NS_MAXDNAME,
						&uptr);
				if (!ptr)
					goto out;

				ptr = uncompress(ntohs(hdr->nscount),
						(char *)reply + offset, eom,
						ptr, uncompressed, NS_MAXDNAME,
						&uptr);
				if (!ptr)
					goto out;

				ptr = uncompress(ntohs(hdr->arcount),
						(char *)reply + offset, eom,
						ptr, uncompressed, NS_MAXDNAME,
						&uptr);
				if (!ptr)
					goto out;

				/*
				 * The uncompressed buffer now contains almost
				 * valid response. Final step is to get rid of
				 * the domain name because at least glibc
				 * gethostbyname() implementation does extra
				 * checks and expects to find an answer without
				 * domain name if we asked a query without
				 * domain part. Note that glibc getaddrinfo()
				 * works differently and accepts FQDN in answer
				 */
				new_len = strip_domains(uncompressed, answers,
							uptr - answers);
				if (new_len < 0) {
					debug("Corrupted packet");
					return -EINVAL;
				}

				/*
				 * Because we have now uncompressed the answers
				 * we might have to create a bigger buffer to
				 * hold all that data.
				 */

				reply_len = header_len + new_len + fixed_len;

				new_reply = g_try_malloc(reply_len);
				if (!new_reply)
					return -ENOMEM;

				memcpy(new_reply, reply, header_len);
				memcpy(new_reply + header_len, uncompressed,
					new_len + fixed_len);

				reply = new_reply;
			}
		}

	pass:
		g_free(req->resp);
		req->resplen = 0;

		req->resp = g_try_malloc(reply_len);
		if (!req->resp)
			return -ENOMEM;

		memcpy(req->resp, reply, reply_len);
		req->resplen = reply_len;

		cache_update(data, reply, reply_len);

		g_free(new_reply);
	}

out:
	if (req->numresp < req->numserv) {
		if (hdr->rcode > ns_r_noerror) {
			return -EINVAL;
		} else if (hdr->ancount == 0 && req->append_domain) {
			return -EINVAL;
		}
	}

	request_list = g_slist_remove(request_list, req);

	if (protocol == IPPROTO_UDP) {
		sk = get_req_udp_socket(req);
		if (sk < 0) {
			errno = -EIO;
			err = -EIO;
		} else
			err = sendto(sk, req->resp, req->resplen, 0,
				&req->sa, req->sa_len);
	} else {
		sk = req->client_sk;
		err = send(sk, req->resp, req->resplen, MSG_NOSIGNAL);
	}

	if (err < 0)
		debug("Cannot send msg, sk %d proto %d errno %d/%s", sk,
			protocol, errno, strerror(errno));
	else
		debug("proto %d sent %d bytes to %d", protocol, err, sk);

	destroy_request_data(req);

	return err;
}