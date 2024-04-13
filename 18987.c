make_new_reply_info(const struct reply_info* rep, struct regional* region,
	size_t an_numrrsets, size_t copy_rrsets)
{
	struct reply_info* new_rep;
	size_t i;

	/* create a base struct.  we specify 'insecure' security status as
	 * the modified response won't be DNSSEC-valid.  In our faked response
	 * the authority and additional sections will be empty (except possible
	 * EDNS0 OPT RR in the additional section appended on sending it out),
	 * so the total number of RRsets is an_numrrsets. */
	new_rep = construct_reply_info_base(region, rep->flags,
		rep->qdcount, rep->ttl, rep->prefetch_ttl,
		rep->serve_expired_ttl, an_numrrsets, 0, 0, an_numrrsets,
		sec_status_insecure);
	if(!new_rep)
		return NULL;
	if(!reply_info_alloc_rrset_keys(new_rep, NULL, region))
		return NULL;
	for(i=0; i<copy_rrsets; i++)
		new_rep->rrsets[i] = rep->rrsets[i];

	return new_rep;
}