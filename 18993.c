respip_data_answer(const struct resp_addr* raddr, enum respip_action action,
	uint16_t qtype, const struct reply_info* rep,
	size_t rrset_id, struct reply_info** new_repp, int tag,
	struct config_strlist** tag_datas, size_t tag_datas_size,
	char* const* tagname, int num_tags,
	struct ub_packed_rrset_key** redirect_rrsetp, struct regional* region)
{
	struct ub_packed_rrset_key* rp = raddr->data;
	struct reply_info* new_rep;
	*redirect_rrsetp = NULL;

	if(action == respip_redirect && tag != -1 &&
		(size_t)tag<tag_datas_size && tag_datas[tag]) {
		struct query_info dataqinfo;
		struct ub_packed_rrset_key r;

		/* Extract parameters of the original answer rrset that can be
		 * rewritten below, in the form of query_info.  Note that these
		 * can be different from the info of the original query if the
		 * rrset is a CNAME target.*/
		memset(&dataqinfo, 0, sizeof(dataqinfo));
		dataqinfo.qname = rep->rrsets[rrset_id]->rk.dname;
		dataqinfo.qname_len = rep->rrsets[rrset_id]->rk.dname_len;
		dataqinfo.qtype = ntohs(rep->rrsets[rrset_id]->rk.type);
		dataqinfo.qclass = ntohs(rep->rrsets[rrset_id]->rk.rrset_class);

		memset(&r, 0, sizeof(r));
		if(local_data_find_tag_datas(&dataqinfo, tag_datas[tag], &r,
			region)) {
			verbose(VERB_ALGO,
				"response-ip redirect with tag data [%d] %s",
				tag, (tag<num_tags?tagname[tag]:"null"));
			/* use copy_rrset() to 'normalize' memory layout */
			rp = copy_rrset(&r, region);
			if(!rp)
				return -1;
		}
	}
	if(!rp)
		return 0;

	/* If we are using response-ip-data, we need to make a copy of rrset
	 * to replace the rrset's dname.  Note that, unlike local data, we
	 * rename the dname for other actions than redirect.  This is because
	 * response-ip-data isn't associated to any specific name. */
	if(rp == raddr->data) {
		rp = copy_rrset(rp, region);
		if(!rp)
			return -1;
		rp->rk.dname = rep->rrsets[rrset_id]->rk.dname;
		rp->rk.dname_len = rep->rrsets[rrset_id]->rk.dname_len;
	}

	/* Build a new reply with redirect rrset.  We keep any preceding CNAMEs
	 * and replace the address rrset that triggers the action.  If it's
	 * type ANY query, however, no other answer records should be kept
	 * (note that it can't be a CNAME chain in this case due to
	 * sanitizing). */
	if(qtype == LDNS_RR_TYPE_ANY)
		rrset_id = 0;
	new_rep = make_new_reply_info(rep, region, rrset_id + 1, rrset_id);
	if(!new_rep)
		return -1;
	rp->rk.flags |= PACKED_RRSET_FIXEDTTL; /* avoid adjusting TTL */
	new_rep->rrsets[rrset_id] = rp;

	*redirect_rrsetp = rp;
	*new_repp = new_rep;
	return 1;
}