static void send_cached_response(int sk, unsigned char *buf, int len,
				const struct sockaddr *to, socklen_t tolen,
				int protocol, int id, uint16_t answers, int ttl)
{
	struct domain_hdr *hdr;
	unsigned char *ptr = buf;
	int err, offset, dns_len, adj_len = len - 2;

	/*
	 * The cached packet contains always the TCP offset (two bytes)
	 * so skip them for UDP.
	 */
	switch (protocol) {
	case IPPROTO_UDP:
		ptr += 2;
		len -= 2;
		dns_len = len;
		offset = 0;
		break;
	case IPPROTO_TCP:
		offset = 2;
		dns_len = ptr[0] * 256 + ptr[1];
		break;
	default:
		return;
	}

	if (len < 12)
		return;

	hdr = (void *) (ptr + offset);

	hdr->id = id;
	hdr->qr = 1;
	hdr->rcode = ns_r_noerror;
	hdr->ancount = htons(answers);
	hdr->nscount = 0;
	hdr->arcount = 0;

	/* if this is a negative reply, we are authoritative */
	if (answers == 0)
		hdr->aa = 1;
	else
		update_cached_ttl((unsigned char *)hdr, adj_len, ttl);

	debug("sk %d id 0x%04x answers %d ptr %p length %d dns %d",
		sk, hdr->id, answers, ptr, len, dns_len);

	err = sendto(sk, ptr, len, MSG_NOSIGNAL, to, tolen);
	if (err < 0) {
		connman_error("Cannot send cached DNS response: %s",
				strerror(errno));
		return;
	}

	if (err != len || (dns_len != (len - 2) && protocol == IPPROTO_TCP) ||
				(dns_len != len && protocol == IPPROTO_UDP))
		debug("Packet length mismatch, sent %d wanted %d dns %d",
			err, len, dns_len);
}