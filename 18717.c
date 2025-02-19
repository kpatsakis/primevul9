static struct xfrm_policy *xfrm_sk_policy_lookup(const struct sock *sk, int dir,
						 const struct flowi *fl,
						 u16 family, u32 if_id)
{
	struct xfrm_policy *pol;

	rcu_read_lock();
 again:
	pol = rcu_dereference(sk->sk_policy[dir]);
	if (pol != NULL) {
		bool match;
		int err = 0;

		if (pol->family != family) {
			pol = NULL;
			goto out;
		}

		match = xfrm_selector_match(&pol->selector, fl, family);
		if (match) {
			if ((sk->sk_mark & pol->mark.m) != pol->mark.v ||
			    pol->if_id != if_id) {
				pol = NULL;
				goto out;
			}
			err = security_xfrm_policy_lookup(pol->security,
						      fl->flowi_secid);
			if (!err) {
				if (!xfrm_pol_hold_rcu(pol))
					goto again;
			} else if (err == -ESRCH) {
				pol = NULL;
			} else {
				pol = ERR_PTR(err);
			}
		} else
			pol = NULL;
	}
out:
	rcu_read_unlock();
	return pol;
}