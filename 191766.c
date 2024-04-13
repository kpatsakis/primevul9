static int netlink_bind(struct socket *sock, struct sockaddr *addr,
			int addr_len)
{
	struct sock *sk = sock->sk;
	struct net *net = sock_net(sk);
	struct netlink_sock *nlk = nlk_sk(sk);
	struct sockaddr_nl *nladdr = (struct sockaddr_nl *)addr;
	int err = 0;
	long unsigned int groups = nladdr->nl_groups;
	bool bound;

	if (addr_len < sizeof(struct sockaddr_nl))
		return -EINVAL;

	if (nladdr->nl_family != AF_NETLINK)
		return -EINVAL;

	/* Only superuser is allowed to listen multicasts */
	if (groups) {
		if (!netlink_allowed(sock, NL_CFG_F_NONROOT_RECV))
			return -EPERM;
		err = netlink_realloc_groups(sk);
		if (err)
			return err;
	}

	bound = nlk->bound;
	if (bound) {
		/* Ensure nlk->portid is up-to-date. */
		smp_rmb();

		if (nladdr->nl_pid != nlk->portid)
			return -EINVAL;
	}

	netlink_lock_table();
	if (nlk->netlink_bind && groups) {
		int group;

		for (group = 0; group < nlk->ngroups; group++) {
			if (!test_bit(group, &groups))
				continue;
			err = nlk->netlink_bind(net, group + 1);
			if (!err)
				continue;
			netlink_undo_bind(group, groups, sk);
			goto unlock;
		}
	}

	/* No need for barriers here as we return to user-space without
	 * using any of the bound attributes.
	 */
	if (!bound) {
		err = nladdr->nl_pid ?
			netlink_insert(sk, nladdr->nl_pid) :
			netlink_autobind(sock);
		if (err) {
			netlink_undo_bind(nlk->ngroups, groups, sk);
			goto unlock;
		}
	}

	if (!groups && (nlk->groups == NULL || !(u32)nlk->groups[0]))
		goto unlock;
	netlink_unlock_table();

	netlink_table_grab();
	netlink_update_subscriptions(sk, nlk->subscriptions +
					 hweight32(groups) -
					 hweight32(nlk->groups[0]));
	nlk->groups[0] = (nlk->groups[0] & ~0xffffffffUL) | groups;
	netlink_update_listeners(sk);
	netlink_table_ungrab();

	return 0;

unlock:
	netlink_unlock_table();
	return err;
}