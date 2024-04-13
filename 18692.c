static void __xfrm_policy_inexact_flush(struct net *net)
{
	struct xfrm_pol_inexact_bin *bin, *t;

	lockdep_assert_held(&net->xfrm.xfrm_policy_lock);

	list_for_each_entry_safe(bin, t, &net->xfrm.inexact_bins, inexact_bins)
		__xfrm_policy_inexact_prune_bin(bin, false);
}