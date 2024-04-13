static void netlink_remove(struct sock *sk)
{
	struct netlink_table *table;

	table = &nl_table[sk->sk_protocol];
	if (!rhashtable_remove_fast(&table->hash, &nlk_sk(sk)->node,
				    netlink_rhashtable_params)) {
		WARN_ON(refcount_read(&sk->sk_refcnt) == 1);
		__sock_put(sk);
	}

	netlink_table_grab();
	if (nlk_sk(sk)->subscriptions) {
		__sk_del_bind_node(sk);
		netlink_update_listeners(sk);
	}
	if (sk->sk_protocol == NETLINK_GENERIC)
		atomic_inc(&genl_sk_destructing_cnt);
	netlink_table_ungrab();
}