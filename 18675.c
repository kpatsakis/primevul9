static void xfrm_policy_inexact_prune_bin(struct xfrm_pol_inexact_bin *b)
{
	struct net *net = read_pnet(&b->k.net);

	spin_lock_bh(&net->xfrm.xfrm_policy_lock);
	__xfrm_policy_inexact_prune_bin(b, false);
	spin_unlock_bh(&net->xfrm.xfrm_policy_lock);
}