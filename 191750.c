int __netlink_change_ngroups(struct sock *sk, unsigned int groups)
{
	struct listeners *new, *old;
	struct netlink_table *tbl = &nl_table[sk->sk_protocol];

	if (groups < 32)
		groups = 32;

	if (NLGRPSZ(tbl->groups) < NLGRPSZ(groups)) {
		new = kzalloc(sizeof(*new) + NLGRPSZ(groups), GFP_ATOMIC);
		if (!new)
			return -ENOMEM;
		old = nl_deref_protected(tbl->listeners);
		memcpy(new->masks, old->masks, NLGRPSZ(tbl->groups));
		rcu_assign_pointer(tbl->listeners, new);

		kfree_rcu(old, rcu);
	}
	tbl->groups = groups;

	return 0;
}