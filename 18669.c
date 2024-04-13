__xfrm_policy_bysel_ctx(struct hlist_head *chain, const struct xfrm_mark *mark,
			u32 if_id, u8 type, int dir, struct xfrm_selector *sel,
			struct xfrm_sec_ctx *ctx)
{
	struct xfrm_policy *pol;

	if (!chain)
		return NULL;

	hlist_for_each_entry(pol, chain, bydst) {
		if (pol->type == type &&
		    pol->if_id == if_id &&
		    xfrm_policy_mark_match(mark, pol) &&
		    !selector_cmp(sel, &pol->selector) &&
		    xfrm_sec_ctx_match(ctx, pol->security))
			return pol;
	}

	return NULL;
}