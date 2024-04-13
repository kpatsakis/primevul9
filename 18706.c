xfrm_policy_inexact_insert(struct xfrm_policy *policy, u8 dir, int excl)
{
	struct xfrm_pol_inexact_bin *bin;
	struct xfrm_policy *delpol;
	struct hlist_head *chain;
	struct net *net;

	bin = xfrm_policy_inexact_alloc_bin(policy, dir);
	if (!bin)
		return ERR_PTR(-ENOMEM);

	net = xp_net(policy);
	lockdep_assert_held(&net->xfrm.xfrm_policy_lock);

	chain = xfrm_policy_inexact_alloc_chain(bin, policy, dir);
	if (!chain) {
		__xfrm_policy_inexact_prune_bin(bin, false);
		return ERR_PTR(-ENOMEM);
	}

	delpol = xfrm_policy_insert_list(chain, policy, excl);
	if (delpol && excl) {
		__xfrm_policy_inexact_prune_bin(bin, false);
		return ERR_PTR(-EEXIST);
	}

	chain = &net->xfrm.policy_inexact[dir];
	xfrm_policy_insert_inexact_list(chain, policy);

	if (delpol)
		__xfrm_policy_inexact_prune_bin(bin, false);

	return delpol;
}