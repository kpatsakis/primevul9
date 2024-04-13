xfrm_policy_inexact_lookup(struct net *net, u8 type, u16 family,
			   u8 dir, u32 if_id)
{
	struct xfrm_pol_inexact_bin *bin;

	lockdep_assert_held(&net->xfrm.xfrm_policy_lock);

	rcu_read_lock();
	bin = xfrm_policy_inexact_lookup_rcu(net, type, family, dir, if_id);
	rcu_read_unlock();

	return bin;
}