generate_cname_request(struct module_qstate* qstate,
	struct ub_packed_rrset_key* alias_rrset)
{
	struct module_qstate* subq = NULL;
	struct query_info subqi;

	memset(&subqi, 0, sizeof(subqi));
	get_cname_target(alias_rrset, &subqi.qname, &subqi.qname_len);
	if(!subqi.qname)
		return 0;    /* unexpected: not a valid CNAME RDATA */
	subqi.qtype = qstate->qinfo.qtype;
	subqi.qclass = qstate->qinfo.qclass;
	fptr_ok(fptr_whitelist_modenv_attach_sub(qstate->env->attach_sub));
	return (*qstate->env->attach_sub)(qstate, &subqi, BIT_RD, 0, 0, &subq);
}