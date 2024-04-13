static inline struct xfrm_dst *xfrm_alloc_dst(struct net *net, int family)
{
	const struct xfrm_policy_afinfo *afinfo = xfrm_policy_get_afinfo(family);
	struct dst_ops *dst_ops;
	struct xfrm_dst *xdst;

	if (!afinfo)
		return ERR_PTR(-EINVAL);

	switch (family) {
	case AF_INET:
		dst_ops = &net->xfrm.xfrm4_dst_ops;
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case AF_INET6:
		dst_ops = &net->xfrm.xfrm6_dst_ops;
		break;
#endif
	default:
		BUG();
	}
	xdst = dst_alloc(dst_ops, NULL, 1, DST_OBSOLETE_NONE, 0);

	if (likely(xdst)) {
		memset_after(xdst, 0, u.dst);
	} else
		xdst = ERR_PTR(-ENOBUFS);

	rcu_read_unlock();

	return xdst;
}