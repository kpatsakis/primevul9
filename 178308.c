static int parse_request(unsigned char *buf, size_t len,
					char *name, unsigned int size)
{
	struct domain_hdr *hdr = (void *) buf;
	uint16_t qdcount = ntohs(hdr->qdcount);
	uint16_t ancount = ntohs(hdr->ancount);
	uint16_t nscount = ntohs(hdr->nscount);
	uint16_t arcount = ntohs(hdr->arcount);
	unsigned char *ptr;
	unsigned int remain, used = 0;

	if (len < sizeof(*hdr) + sizeof(struct qtype_qclass) ||
			hdr->qr || qdcount != 1 || ancount || nscount) {
		DBG("Dropped DNS request qr %d with len %zd qdcount %d "
			"ancount %d nscount %d", hdr->qr, len, qdcount, ancount,
			nscount);

		return -EINVAL;
	}

	if (!name || !size)
		return -EINVAL;

	debug("id 0x%04x qr %d opcode %d qdcount %d arcount %d",
					hdr->id, hdr->qr, hdr->opcode,
							qdcount, arcount);

	name[0] = '\0';

	ptr = buf + sizeof(struct domain_hdr);
	remain = len - sizeof(struct domain_hdr);

	while (remain > 0) {
		uint8_t label_len = *ptr;

		if (label_len == 0x00) {
			uint8_t class;
			struct qtype_qclass *q =
				(struct qtype_qclass *)(ptr + 1);

			if (remain < sizeof(*q)) {
				DBG("Dropped malformed DNS query");
				return -EINVAL;
			}

			class = ntohs(q->qclass);
			if (class != 1 && class != 255) {
				DBG("Dropped non-IN DNS class %d", class);
				return -EINVAL;
			}

			ptr += sizeof(*q) + 1;
			remain -= (sizeof(*q) + 1);
			break;
		}

		if (used + label_len + 1 > size)
			return -ENOBUFS;

		strncat(name, (char *) (ptr + 1), label_len);
		strcat(name, ".");

		used += label_len + 1;

		ptr += label_len + 1;
		remain -= label_len + 1;
	}

	if (arcount && remain >= sizeof(struct domain_rr) + 1 && !ptr[0] &&
		ptr[1] == opt_edns0_type[0] && ptr[2] == opt_edns0_type[1]) {
		struct domain_rr *edns0 = (struct domain_rr *)(ptr + 1);

		DBG("EDNS0 buffer size %u", ntohs(edns0->class));
	} else if (!arcount && remain) {
		DBG("DNS request with %d garbage bytes", remain);
	}

	debug("query %s", name);

	return 0;
}