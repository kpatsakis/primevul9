struct dst_entry *xfrm_lookup_route(struct net *net, struct dst_entry *dst_orig,
				    const struct flowi *fl,
				    const struct sock *sk, int flags)
{
	struct dst_entry *dst = xfrm_lookup(net, dst_orig, fl, sk,
					    flags | XFRM_LOOKUP_QUEUE |
					    XFRM_LOOKUP_KEEP_DST_REF);

	if (PTR_ERR(dst) == -EREMOTE)
		return make_blackhole(net, dst_orig->ops->family, dst_orig);

	if (IS_ERR(dst))
		dst_release(dst_orig);

	return dst;
}