respip_rewrite_reply(const struct query_info* qinfo,
	const struct respip_client_info* cinfo, const struct reply_info* rep,
	struct reply_info** new_repp, struct respip_action_info* actinfo,
	struct ub_packed_rrset_key** alias_rrset, int search_only,
	struct regional* region)
{
	const uint8_t* ctaglist;
	size_t ctaglen;
	const uint8_t* tag_actions;
	size_t tag_actions_size;
	struct config_strlist** tag_datas;
	size_t tag_datas_size;
	struct view* view = NULL;
	struct respip_set* ipset = NULL;
	size_t rrset_id = 0;
	enum respip_action action = respip_none;
	int tag = -1;
	const struct resp_addr* raddr = NULL;
	int ret = 1;
	struct ub_packed_rrset_key* redirect_rrset = NULL;

	if(!cinfo)
		goto done;
	ctaglist = cinfo->taglist;
	ctaglen = cinfo->taglen;
	tag_actions = cinfo->tag_actions;
	tag_actions_size = cinfo->tag_actions_size;
	tag_datas = cinfo->tag_datas;
	tag_datas_size = cinfo->tag_datas_size;
	view = cinfo->view;
	ipset = cinfo->respip_set;

	/** Try to use response-ip config from the view first; use
	  * global response-ip config if we don't have the view or we don't
	  * have the matching per-view config (and the view allows the use
	  * of global data in this case).
	  * Note that we lock the view even if we only use view members that
	  * currently don't change after creation.  This is for safety for
	  * future possible changes as the view documentation seems to expect
	  * any of its member can change in the view's lifetime.
	  * Note also that we assume 'view' is valid in this function, which
	  * should be safe (see unbound bug #1191) */
	if(view) {
		lock_rw_rdlock(&view->lock);
		if(view->respip_set) {
			if((raddr = respip_addr_lookup(rep,
				&view->respip_set->ip_tree, &rrset_id))) {
				/** for per-view respip directives the action
				 * can only be direct (i.e. not tag-based) */
				action = raddr->action;
			}
		}
		if(!raddr && !view->isfirst)
			goto done;
	}
	if(!raddr && ipset && (raddr = respip_addr_lookup(rep, &ipset->ip_tree,
		&rrset_id))) {
		action = (enum respip_action)local_data_find_tag_action(
			raddr->taglist, raddr->taglen, ctaglist, ctaglen,
			tag_actions, tag_actions_size,
			(enum localzone_type)raddr->action, &tag,
			ipset->tagname, ipset->num_tags);
	}
	if(raddr && !search_only) {
		int result = 0;

		/* first, see if we have response-ip or tag action for the
		 * action except for 'always' variants. */
		if(action != respip_always_refuse
			&& action != respip_always_transparent
			&& action != respip_always_nxdomain
			&& (result = respip_data_answer(raddr, action,
			qinfo->qtype, rep, rrset_id, new_repp, tag, tag_datas,
			tag_datas_size, ipset->tagname, ipset->num_tags,
			&redirect_rrset, region)) < 0) {
			ret = 0;
			goto done;
		}

		/* if no action data applied, take action specific to the
		 * action without data. */
		if(!result && !respip_nodata_answer(qinfo->qtype, action, rep,
			rrset_id, new_repp, region)) {
			ret = 0;
			goto done;
		}
	}
  done:
	if(view) {
		lock_rw_unlock(&view->lock);
	}
	if(ret) {
		/* If we're redirecting the original answer to a
		 * CNAME, record the CNAME rrset so the caller can take
		 * the appropriate action.  Note that we don't check the
		 * action type; it should normally be 'redirect', but it
		 * can be of other type when a data-dependent tag action
		 * uses redirect response-ip data.
		 */
		if(redirect_rrset &&
			redirect_rrset->rk.type == ntohs(LDNS_RR_TYPE_CNAME) &&
			qinfo->qtype != LDNS_RR_TYPE_ANY)
			*alias_rrset = redirect_rrset;
		/* on success, populate respip result structure */
		ret = populate_action_info(actinfo, action, raddr,
			redirect_rrset, tag, ipset, search_only, region);
	}
	return ret;
}