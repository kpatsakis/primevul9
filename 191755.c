static struct sock *__netlink_lookup(struct netlink_table *table, u32 portid,
				     struct net *net)
{
	struct netlink_compare_arg arg;

	netlink_compare_arg_init(&arg, net, portid);
	return rhashtable_lookup_fast(&table->hash, &arg,
				      netlink_rhashtable_params);
}