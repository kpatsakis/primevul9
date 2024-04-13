respip_operate(struct module_qstate* qstate, enum module_ev event, int id,
	struct outbound_entry* outbound)
{
	struct respip_qstate* rq = (struct respip_qstate*)qstate->minfo[id];

	log_query_info(VERB_QUERY, "respip operate: query", &qstate->qinfo);
	(void)outbound;

	if(event == module_event_new || event == module_event_pass) {
		if(!rq) {
			rq = regional_alloc_zero(qstate->region, sizeof(*rq));
			if(!rq)
				goto servfail;
			rq->state = RESPIP_INIT;
			qstate->minfo[id] = rq;
		}
		if(rq->state == RESPIP_SUBQUERY_FINISHED) {
			qstate->ext_state[id] = module_finished;
			return;
		}
		verbose(VERB_ALGO, "respip: pass to next module");
		qstate->ext_state[id] = module_wait_module;
	} else if(event == module_event_moddone) {
		/* If the reply may be subject to response-ip rewriting
		 * according to the query type, check the actions.  If a
		 * rewrite is necessary, we'll replace the reply in qstate
		 * with the new one. */
		enum module_ext_state next_state = module_finished;

		if((qstate->qinfo.qtype == LDNS_RR_TYPE_A ||
			qstate->qinfo.qtype == LDNS_RR_TYPE_AAAA ||
			qstate->qinfo.qtype == LDNS_RR_TYPE_ANY) &&
			qstate->return_msg && qstate->return_msg->rep) {
			struct respip_action_info actinfo = {respip_none, NULL};
			struct reply_info* new_rep = qstate->return_msg->rep;
			struct ub_packed_rrset_key* alias_rrset = NULL;

			if(!respip_rewrite_reply(&qstate->qinfo,
				qstate->client_info, qstate->return_msg->rep,
				&new_rep, &actinfo, &alias_rrset, 0,
				qstate->region)) {
				goto servfail;
			}
			if(actinfo.action != respip_none) {
				/* save action info for logging on a
				 * per-front-end-query basis */
				if(!(qstate->respip_action_info =
					regional_alloc_init(qstate->region,
						&actinfo, sizeof(actinfo))))
				{
					log_err("out of memory");
					goto servfail;
				}
			} else {
				qstate->respip_action_info = NULL;
			}
			if (new_rep == qstate->return_msg->rep &&
				(actinfo.action == respip_deny ||
				actinfo.action == respip_inform_deny)) {
				/* for deny-variant actions (unless response-ip
				 * data is applied), mark the query state so
				 * the response will be dropped for all
				 * clients. */
				qstate->is_drop = 1;
			} else if(alias_rrset) {
				if(!generate_cname_request(qstate, alias_rrset))
					goto servfail;
				next_state = module_wait_subquery;
			}
			qstate->return_msg->rep = new_rep;
		}
		qstate->ext_state[id] = next_state;
	} else
		qstate->ext_state[id] = module_finished;

	return;

  servfail:
	qstate->return_rcode = LDNS_RCODE_SERVFAIL;
	qstate->return_msg = NULL;
}