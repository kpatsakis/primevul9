static int __netlink_insert(struct netlink_table *table, struct sock *sk)
{
	struct netlink_compare_arg arg;

	netlink_compare_arg_init(&arg, sock_net(sk), nlk_sk(sk)->portid);
	return rhashtable_lookup_insert_key(&table->hash, &arg,
					    &nlk_sk(sk)->node,
					    netlink_rhashtable_params);
}