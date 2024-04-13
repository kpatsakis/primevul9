static struct xfrm_policy *xfrm_policy_lookup(struct net *net,
					      const struct flowi *fl,
					      u16 family, u8 dir, u32 if_id)
{
#ifdef CONFIG_XFRM_SUB_POLICY
	struct xfrm_policy *pol;

	pol = xfrm_policy_lookup_bytype(net, XFRM_POLICY_TYPE_SUB, fl, family,
					dir, if_id);
	if (pol != NULL)
		return pol;
#endif
	return xfrm_policy_lookup_bytype(net, XFRM_POLICY_TYPE_MAIN, fl, family,
					 dir, if_id);
}