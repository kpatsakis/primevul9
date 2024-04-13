static int netlink_autobind(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct net *net = sock_net(sk);
	struct netlink_table *table = &nl_table[sk->sk_protocol];
	s32 portid = task_tgid_vnr(current);
	int err;
	s32 rover = -4096;
	bool ok;

retry:
	cond_resched();
	rcu_read_lock();
	ok = !__netlink_lookup(table, portid, net);
	rcu_read_unlock();
	if (!ok) {
		/* Bind collision, search negative portid values. */
		if (rover == -4096)
			/* rover will be in range [S32_MIN, -4097] */
			rover = S32_MIN + prandom_u32_max(-4096 - S32_MIN);
		else if (rover >= -4096)
			rover = -4097;
		portid = rover--;
		goto retry;
	}

	err = netlink_insert(sk, portid);
	if (err == -EADDRINUSE)
		goto retry;

	/* If 2 threads race to autobind, that is fine.  */
	if (err == -EBUSY)
		err = 0;

	return err;
}