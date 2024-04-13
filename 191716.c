static struct sock *netlink_lookup(struct net *net, int protocol, u32 portid)
{
	struct netlink_table *table = &nl_table[protocol];
	struct sock *sk;

	rcu_read_lock();
	sk = __netlink_lookup(table, portid, net);
	if (sk)
		sock_hold(sk);
	rcu_read_unlock();

	return sk;
}