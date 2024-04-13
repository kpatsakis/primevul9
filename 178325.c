static void send_response(int sk, unsigned char *buf, size_t len,
				const struct sockaddr *to, socklen_t tolen,
				int protocol)
{
	struct domain_hdr *hdr;
	int err, offset = protocol_offset(protocol);

	debug("sk %d", sk);

	if (offset < 0)
		return;

	if (len < sizeof(*hdr) + offset)
		return;

	hdr = (void *) (buf + offset);
	if (offset) {
		buf[0] = 0;
		buf[1] = sizeof(*hdr);
	}

	debug("id 0x%04x qr %d opcode %d", hdr->id, hdr->qr, hdr->opcode);

	hdr->qr = 1;
	hdr->rcode = ns_r_servfail;

	hdr->qdcount = 0;
	hdr->ancount = 0;
	hdr->nscount = 0;
	hdr->arcount = 0;

	err = sendto(sk, buf, sizeof(*hdr) + offset, MSG_NOSIGNAL, to, tolen);
	if (err < 0) {
		connman_error("Failed to send DNS response to %d: %s",
				sk, strerror(errno));
		return;
	}
}