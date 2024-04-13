int xfrm_sk_policy_insert(struct sock *sk, int dir, struct xfrm_policy *pol)
{
	struct net *net = sock_net(sk);
	struct xfrm_policy *old_pol;

#ifdef CONFIG_XFRM_SUB_POLICY
	if (pol && pol->type != XFRM_POLICY_TYPE_MAIN)
		return -EINVAL;
#endif

	spin_lock_bh(&net->xfrm.xfrm_policy_lock);
	old_pol = rcu_dereference_protected(sk->sk_policy[dir],
				lockdep_is_held(&net->xfrm.xfrm_policy_lock));
	if (pol) {
		pol->curlft.add_time = ktime_get_real_seconds();
		pol->index = xfrm_gen_index(net, XFRM_POLICY_MAX+dir, 0);
		xfrm_sk_policy_link(pol, dir);
	}
	rcu_assign_pointer(sk->sk_policy[dir], pol);
	if (old_pol) {
		if (pol)
			xfrm_policy_requeue(old_pol, pol);

		/* Unlinking succeeds always. This is the only function
		 * allowed to delete or replace socket policy.
		 */
		xfrm_sk_policy_unlink(old_pol, dir);
	}
	spin_unlock_bh(&net->xfrm.xfrm_policy_lock);

	if (old_pol) {
		xfrm_policy_kill(old_pol);
	}
	return 0;
}