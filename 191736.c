int netlink_has_listeners(struct sock *sk, unsigned int group)
{
	int res = 0;
	struct listeners *listeners;

	BUG_ON(!netlink_is_kernel(sk));

	rcu_read_lock();
	listeners = rcu_dereference(nl_table[sk->sk_protocol].listeners);

	if (listeners && group - 1 < nl_table[sk->sk_protocol].groups)
		res = test_bit(group - 1, listeners->masks);

	rcu_read_unlock();

	return res;
}