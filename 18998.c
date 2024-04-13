respip_addr_lookup(const struct reply_info *rep, struct rbtree_type* iptree,
	size_t* rrset_id)
{
	size_t i;
	struct resp_addr* ra;
	struct sockaddr_storage ss;
	socklen_t addrlen;

	for(i=0; i<rep->an_numrrsets; i++) {
		size_t j;
		const struct packed_rrset_data* rd;
		uint16_t rtype = ntohs(rep->rrsets[i]->rk.type);

		if(rtype != LDNS_RR_TYPE_A && rtype != LDNS_RR_TYPE_AAAA)
			continue;
		rd = rep->rrsets[i]->entry.data;
		for(j = 0; j < rd->count; j++) {
			if(!rdata2sockaddr(rd, rtype, j, &ss, &addrlen))
				continue;
			ra = (struct resp_addr*)addr_tree_lookup(iptree, &ss,
				addrlen);
			if(ra) {
				*rrset_id = i;
				return ra;
			}
		}
	}

	return NULL;
}