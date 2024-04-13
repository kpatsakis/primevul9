new_rrset(struct regional* region, uint16_t rrtype, uint16_t rrclass)
{
	struct packed_rrset_data* pd;
	struct ub_packed_rrset_key* rrset = regional_alloc_zero(
		region, sizeof(*rrset));
	if(!rrset) {
		log_err("out of memory");
		return NULL;
	}
	rrset->entry.key = rrset;
	pd = regional_alloc_zero(region, sizeof(*pd));
	if(!pd) {
		log_err("out of memory");
		return NULL;
	}
	pd->trust = rrset_trust_prim_noglue;
	pd->security = sec_status_insecure;
	rrset->entry.data = pd;
	rrset->rk.dname = regional_alloc_zero(region, 1);
	if(!rrset->rk.dname) {
		log_err("out of memory");
		return NULL;
	}
	rrset->rk.dname_len = 1;
	rrset->rk.type = htons(rrtype);
	rrset->rk.rrset_class = htons(rrclass);
	return rrset;
}