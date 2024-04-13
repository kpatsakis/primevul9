static int netlink_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct netlink_sock *nlk;

	if (!sk)
		return 0;

	netlink_remove(sk);
	sock_orphan(sk);
	nlk = nlk_sk(sk);

	/*
	 * OK. Socket is unlinked, any packets that arrive now
	 * will be purged.
	 */

	/* must not acquire netlink_table_lock in any way again before unbind
	 * and notifying genetlink is done as otherwise it might deadlock
	 */
	if (nlk->netlink_unbind) {
		int i;

		for (i = 0; i < nlk->ngroups; i++)
			if (test_bit(i, nlk->groups))
				nlk->netlink_unbind(sock_net(sk), i + 1);
	}
	if (sk->sk_protocol == NETLINK_GENERIC &&
	    atomic_dec_return(&genl_sk_destructing_cnt) == 0)
		wake_up(&genl_sk_destructing_waitq);

	sock->sk = NULL;
	wake_up_interruptible_all(&nlk->wait);

	skb_queue_purge(&sk->sk_write_queue);

	if (nlk->portid && nlk->bound) {
		struct netlink_notify n = {
						.net = sock_net(sk),
						.protocol = sk->sk_protocol,
						.portid = nlk->portid,
					  };
		blocking_notifier_call_chain(&netlink_chain,
				NETLINK_URELEASE, &n);
	}

	module_put(nlk->module);

	if (netlink_is_kernel(sk)) {
		netlink_table_grab();
		BUG_ON(nl_table[sk->sk_protocol].registered == 0);
		if (--nl_table[sk->sk_protocol].registered == 0) {
			struct listeners *old;

			old = nl_deref_protected(nl_table[sk->sk_protocol].listeners);
			RCU_INIT_POINTER(nl_table[sk->sk_protocol].listeners, NULL);
			kfree_rcu(old, rcu);
			nl_table[sk->sk_protocol].module = NULL;
			nl_table[sk->sk_protocol].bind = NULL;
			nl_table[sk->sk_protocol].unbind = NULL;
			nl_table[sk->sk_protocol].flags = 0;
			nl_table[sk->sk_protocol].registered = 0;
		}
		netlink_table_ungrab();
	}

	local_bh_disable();
	sock_prot_inuse_add(sock_net(sk), &netlink_proto, -1);
	local_bh_enable();
	call_rcu(&nlk->rcu, deferred_put_nlk_sk);
	return 0;
}