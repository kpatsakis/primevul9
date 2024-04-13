static void xfrm_policy_inexact_list_reinsert(struct net *net,
					      struct xfrm_pol_inexact_node *n,
					      u16 family)
{
	unsigned int matched_s, matched_d;
	struct xfrm_policy *policy, *p;

	matched_s = 0;
	matched_d = 0;

	list_for_each_entry_reverse(policy, &net->xfrm.policy_all, walk.all) {
		struct hlist_node *newpos = NULL;
		bool matches_s, matches_d;

		if (!policy->bydst_reinsert)
			continue;

		WARN_ON_ONCE(policy->family != family);

		policy->bydst_reinsert = false;
		hlist_for_each_entry(p, &n->hhead, bydst) {
			if (policy->priority > p->priority)
				newpos = &p->bydst;
			else if (policy->priority == p->priority &&
				 policy->pos > p->pos)
				newpos = &p->bydst;
			else
				break;
		}

		if (newpos)
			hlist_add_behind_rcu(&policy->bydst, newpos);
		else
			hlist_add_head_rcu(&policy->bydst, &n->hhead);

		/* paranoia checks follow.
		 * Check that the reinserted policy matches at least
		 * saddr or daddr for current node prefix.
		 *
		 * Matching both is fine, matching saddr in one policy
		 * (but not daddr) and then matching only daddr in another
		 * is a bug.
		 */
		matches_s = xfrm_policy_addr_delta(&policy->selector.saddr,
						   &n->addr,
						   n->prefixlen,
						   family) == 0;
		matches_d = xfrm_policy_addr_delta(&policy->selector.daddr,
						   &n->addr,
						   n->prefixlen,
						   family) == 0;
		if (matches_s && matches_d)
			continue;

		WARN_ON_ONCE(!matches_s && !matches_d);
		if (matches_s)
			matched_s++;
		if (matches_d)
			matched_d++;
		WARN_ON_ONCE(matched_s && matched_d);
	}
}