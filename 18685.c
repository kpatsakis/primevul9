static void xfrm_policy_insert_inexact_list(struct hlist_head *chain,
					    struct xfrm_policy *policy)
{
	struct xfrm_policy *pol, *delpol = NULL;
	struct hlist_node *newpos = NULL;
	int i = 0;

	hlist_for_each_entry(pol, chain, bydst_inexact_list) {
		if (pol->type == policy->type &&
		    pol->if_id == policy->if_id &&
		    !selector_cmp(&pol->selector, &policy->selector) &&
		    xfrm_policy_mark_match(&policy->mark, pol) &&
		    xfrm_sec_ctx_match(pol->security, policy->security) &&
		    !WARN_ON(delpol)) {
			delpol = pol;
			if (policy->priority > pol->priority)
				continue;
		} else if (policy->priority >= pol->priority) {
			newpos = &pol->bydst_inexact_list;
			continue;
		}
		if (delpol)
			break;
	}

	if (newpos)
		hlist_add_behind_rcu(&policy->bydst_inexact_list, newpos);
	else
		hlist_add_head_rcu(&policy->bydst_inexact_list, chain);

	hlist_for_each_entry(pol, chain, bydst_inexact_list) {
		pol->pos = i;
		i++;
	}
}