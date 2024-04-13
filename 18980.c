respip_nodata_answer(uint16_t qtype, enum respip_action action,
	const struct reply_info *rep, size_t rrset_id,
	struct reply_info** new_repp, struct regional* region)
{
	struct reply_info* new_rep;

	if(action == respip_refuse || action == respip_always_refuse) {
		new_rep = make_new_reply_info(rep, region, 0, 0);
		if(!new_rep)
			return 0;
		FLAGS_SET_RCODE(new_rep->flags, LDNS_RCODE_REFUSED);
		*new_repp = new_rep;
		return 1;
	} else if(action == respip_static || action == respip_redirect ||
		action == respip_always_nxdomain ||
		action == respip_inform_redirect) {
		/* Since we don't know about other types of the owner name,
		 * we generally return NOERROR/NODATA unless an NXDOMAIN action
		 * is explicitly specified. */
		int rcode = (action == respip_always_nxdomain)?
			LDNS_RCODE_NXDOMAIN:LDNS_RCODE_NOERROR;

		/* We should empty the answer section except for any preceding
		 * CNAMEs (in that case rrset_id > 0).  Type-ANY case is
		 * special as noted in respip_data_answer(). */
		if(qtype == LDNS_RR_TYPE_ANY)
			rrset_id = 0;
		new_rep = make_new_reply_info(rep, region, rrset_id, rrset_id);
		if(!new_rep)
			return 0;
		FLAGS_SET_RCODE(new_rep->flags, rcode);
		*new_repp = new_rep;
		return 1;
	}

	return 1;
}