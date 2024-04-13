xfrm_secpath_reject(int idx, struct sk_buff *skb, const struct flowi *fl)
{
	struct sec_path *sp = skb_sec_path(skb);
	struct xfrm_state *x;

	if (!sp || idx < 0 || idx >= sp->len)
		return 0;
	x = sp->xvec[idx];
	if (!x->type->reject)
		return 0;
	return x->type->reject(x, skb, fl);
}