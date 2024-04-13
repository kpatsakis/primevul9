netlink_update_listeners(struct sock *sk)
{
	struct netlink_table *tbl = &nl_table[sk->sk_protocol];
	unsigned long mask;
	unsigned int i;
	struct listeners *listeners;

	listeners = nl_deref_protected(tbl->listeners);
	if (!listeners)
		return;

	for (i = 0; i < NLGRPLONGS(tbl->groups); i++) {
		mask = 0;
		sk_for_each_bound(sk, &tbl->mc_list) {
			if (i < NLGRPLONGS(nlk_sk(sk)->ngroups))
				mask |= nlk_sk(sk)->groups[i];
		}
		listeners->masks[i] = mask;
	}
	/* this function is only called with the netlink table "grabbed", which
	 * makes sure updates are visible before bind or setsockopt return. */
}