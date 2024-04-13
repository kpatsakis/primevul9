xfrm_policy_byid(struct net *net, const struct xfrm_mark *mark, u32 if_id,
		 u8 type, int dir, u32 id, int delete, int *err)
{
	struct xfrm_policy *pol, *ret;
	struct hlist_head *chain;

	*err = -ENOENT;
	if (xfrm_policy_id2dir(id) != dir)
		return NULL;

	*err = 0;
	spin_lock_bh(&net->xfrm.xfrm_policy_lock);
	chain = net->xfrm.policy_byidx + idx_hash(net, id);
	ret = NULL;
	hlist_for_each_entry(pol, chain, byidx) {
		if (pol->type == type && pol->index == id &&
		    pol->if_id == if_id && xfrm_policy_mark_match(mark, pol)) {
			xfrm_pol_hold(pol);
			if (delete) {
				*err = security_xfrm_policy_delete(
								pol->security);
				if (*err) {
					spin_unlock_bh(&net->xfrm.xfrm_policy_lock);
					return pol;
				}
				__xfrm_policy_unlink(pol, dir);
			}
			ret = pol;
			break;
		}
	}
	spin_unlock_bh(&net->xfrm.xfrm_policy_lock);

	if (ret && delete)
		xfrm_policy_kill(ret);
	return ret;
}