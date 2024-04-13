rdata2sockaddr(const struct packed_rrset_data* rd, uint16_t rtype, size_t i,
	struct sockaddr_storage* ss, socklen_t* addrlenp)
{
	/* unbound can accept and cache odd-length AAAA/A records, so we have
	 * to validate the length. */
	if(rtype == LDNS_RR_TYPE_A && rd->rr_len[i] == 6) {
		struct sockaddr_in* sa4 = (struct sockaddr_in*)ss;

		memset(sa4, 0, sizeof(*sa4));
		sa4->sin_family = AF_INET;
		memcpy(&sa4->sin_addr, rd->rr_data[i] + 2,
			sizeof(sa4->sin_addr));
		*addrlenp = sizeof(*sa4);
		return 1;
	} else if(rtype == LDNS_RR_TYPE_AAAA && rd->rr_len[i] == 18) {
		struct sockaddr_in6* sa6 = (struct sockaddr_in6*)ss;

		memset(sa6, 0, sizeof(*sa6));
		sa6->sin6_family = AF_INET6;
		memcpy(&sa6->sin6_addr, rd->rr_data[i] + 2,
			sizeof(sa6->sin6_addr));
		*addrlenp = sizeof(*sa6);
		return 1;
	}
	return 0;
}