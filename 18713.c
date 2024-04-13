static struct xfrm_policy *xfrm_policy_insert_list(struct hlist_head *chain,
						   struct xfrm_policy *policy,
						   bool excl)
{
	struct xfrm_policy *pol, *newpos = NULL, *delpol = NULL;

	hlist_for_each_entry(pol, chain, bydst) {
		if (pol->type == policy->type &&
		    pol->if_id == policy->if_id &&
		    !selector_cmp(&pol->selector, &policy->selector) &&
		    xfrm_policy_mark_match(&policy->mark, pol) &&
		    xfrm_sec_ctx_match(pol->security, policy->security) &&
		    !WARN_ON(delpol)) {
			if (excl)
				return ERR_PTR(-EEXIST);
			delpol = pol;
			if (policy->priority > pol->priority)
				continue;
		} else if (policy->priority >= pol->priority) {
			newpos = pol;
			continue;
		}
		if (delpol)
			break;
	}

	if (newpos)
		hlist_add_behind_rcu(&policy->bydst, &newpos->bydst);
	else
		hlist_add_head_rcu(&policy->bydst, chain);

	return delpol;
}