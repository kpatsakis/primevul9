static struct xfrm_policy *xfrm_migrate_policy_find(const struct xfrm_selector *sel,
						    u8 dir, u8 type, struct net *net, u32 if_id)
{
	struct xfrm_policy *pol, *ret = NULL;
	struct hlist_head *chain;
	u32 priority = ~0U;

	spin_lock_bh(&net->xfrm.xfrm_policy_lock);
	chain = policy_hash_direct(net, &sel->daddr, &sel->saddr, sel->family, dir);
	hlist_for_each_entry(pol, chain, bydst) {
		if ((if_id == 0 || pol->if_id == if_id) &&
		    xfrm_migrate_selector_match(sel, &pol->selector) &&
		    pol->type == type) {
			ret = pol;
			priority = ret->priority;
			break;
		}
	}
	chain = &net->xfrm.policy_inexact[dir];
	hlist_for_each_entry(pol, chain, bydst_inexact_list) {
		if ((pol->priority >= priority) && ret)
			break;

		if ((if_id == 0 || pol->if_id == if_id) &&
		    xfrm_migrate_selector_match(sel, &pol->selector) &&
		    pol->type == type) {
			ret = pol;
			break;
		}
	}

	xfrm_pol_hold(ret);

	spin_unlock_bh(&net->xfrm.xfrm_policy_lock);

	return ret;
}