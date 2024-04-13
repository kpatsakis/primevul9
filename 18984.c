respip_inform_super(struct module_qstate* qstate, int id,
	struct module_qstate* super)
{
	struct respip_qstate* rq = (struct respip_qstate*)super->minfo[id];
	struct reply_info* new_rep = NULL;

	rq->state = RESPIP_SUBQUERY_FINISHED;

	/* respip subquery should have always been created with a valid reply
	 * in super. */
	log_assert(super->return_msg && super->return_msg->rep);

	/* return_msg can be NULL when, e.g., the sub query resulted in
	 * SERVFAIL, in which case we regard it as a failure of the original
	 * query.  Other checks are probably redundant, but we check them
	 * for safety. */
	if(!qstate->return_msg || !qstate->return_msg->rep ||
		qstate->return_rcode != LDNS_RCODE_NOERROR)
		goto fail;

	if(!respip_merge_cname(super->return_msg->rep, &qstate->qinfo,
		qstate->return_msg->rep, super->client_info,
		super->env->need_to_validate, &new_rep, super->region))
		goto fail;
	super->return_msg->rep = new_rep;
	return;

  fail:
	super->return_rcode = LDNS_RCODE_SERVFAIL;
	super->return_msg = NULL;
	return;
}