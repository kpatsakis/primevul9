xfrm_policy_inexact_alloc_chain(struct xfrm_pol_inexact_bin *bin,
				struct xfrm_policy *policy, u8 dir)
{
	struct xfrm_pol_inexact_node *n;
	struct net *net;

	net = xp_net(policy);
	lockdep_assert_held(&net->xfrm.xfrm_policy_lock);

	if (xfrm_policy_inexact_insert_use_any_list(policy))
		return &bin->hhead;

	if (xfrm_pol_inexact_addr_use_any_list(&policy->selector.daddr,
					       policy->family,
					       policy->selector.prefixlen_d)) {
		write_seqcount_begin(&bin->count);
		n = xfrm_policy_inexact_insert_node(net,
						    &bin->root_s,
						    &policy->selector.saddr,
						    policy->family,
						    policy->selector.prefixlen_s,
						    dir);
		write_seqcount_end(&bin->count);
		if (!n)
			return NULL;

		return &n->hhead;
	}

	/* daddr is fixed */
	write_seqcount_begin(&bin->count);
	n = xfrm_policy_inexact_insert_node(net,
					    &bin->root_d,
					    &policy->selector.daddr,
					    policy->family,
					    policy->selector.prefixlen_d, dir);
	write_seqcount_end(&bin->count);
	if (!n)
		return NULL;

	/* saddr is wildcard */
	if (xfrm_pol_inexact_addr_use_any_list(&policy->selector.saddr,
					       policy->family,
					       policy->selector.prefixlen_s))
		return &n->hhead;

	write_seqcount_begin(&bin->count);
	n = xfrm_policy_inexact_insert_node(net,
					    &n->root,
					    &policy->selector.saddr,
					    policy->family,
					    policy->selector.prefixlen_s, dir);
	write_seqcount_end(&bin->count);
	if (!n)
		return NULL;

	return &n->hhead;
}