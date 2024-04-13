struct dst_entry *xfrm_lookup(struct net *net, struct dst_entry *dst_orig,
			      const struct flowi *fl, const struct sock *sk,
			      int flags)
{
	return xfrm_lookup_with_ifid(net, dst_orig, fl, sk, flags, 0);
}