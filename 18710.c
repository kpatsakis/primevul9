xfrm_policy_bysel_ctx(struct net *net, const struct xfrm_mark *mark, u32 if_id,
		      u8 type, int dir, struct xfrm_selector *sel,
		      struct xfrm_sec_ctx *ctx, int delete, int *err)
{
	struct xfrm_pol_inexact_bin *bin = NULL;
	struct xfrm_policy *pol, *ret = NULL;
	struct hlist_head *chain;

	*err = 0;
	spin_lock_bh(&net->xfrm.xfrm_policy_lock);
	chain = policy_hash_bysel(net, sel, sel->family, dir);
	if (!chain) {
		struct xfrm_pol_inexact_candidates cand;
		int i;

		bin = xfrm_policy_inexact_lookup(net, type,
						 sel->family, dir, if_id);
		if (!bin) {
			spin_unlock_bh(&net->xfrm.xfrm_policy_lock);
			return NULL;
		}

		if (!xfrm_policy_find_inexact_candidates(&cand, bin,
							 &sel->saddr,
							 &sel->daddr)) {
			spin_unlock_bh(&net->xfrm.xfrm_policy_lock);
			return NULL;
		}

		pol = NULL;
		for (i = 0; i < ARRAY_SIZE(cand.res); i++) {
			struct xfrm_policy *tmp;

			tmp = __xfrm_policy_bysel_ctx(cand.res[i], mark,
						      if_id, type, dir,
						      sel, ctx);
			if (!tmp)
				continue;

			if (!pol || tmp->pos < pol->pos)
				pol = tmp;
		}
	} else {
		pol = __xfrm_policy_bysel_ctx(chain, mark, if_id, type, dir,
					      sel, ctx);
	}

	if (pol) {
		xfrm_pol_hold(pol);
		if (delete) {
			*err = security_xfrm_policy_delete(pol->security);
			if (*err) {
				spin_unlock_bh(&net->xfrm.xfrm_policy_lock);
				return pol;
			}
			__xfrm_policy_unlink(pol, dir);
		}
		ret = pol;
	}
	spin_unlock_bh(&net->xfrm.xfrm_policy_lock);

	if (ret && delete)
		xfrm_policy_kill(ret);
	if (bin && delete)
		xfrm_policy_inexact_prune_bin(bin);
	return ret;
}