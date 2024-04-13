static struct rt6_info *rt6_get_route_info(struct net *net,
					   const struct in6_addr *prefix, int prefixlen,
					   const struct in6_addr *gwaddr, int ifindex)
{
	struct fib6_node *fn;
	struct rt6_info *rt = NULL;
	struct fib6_table *table;

	table = fib6_get_table(net, RT6_TABLE_INFO);
	if (!table)
		return NULL;

	read_lock_bh(&table->tb6_lock);
	fn = fib6_locate(&table->tb6_root, prefix, prefixlen, NULL, 0);
	if (!fn)
		goto out;

	for (rt = fn->leaf; rt; rt = rt->dst.rt6_next) {
		if (rt->dst.dev->ifindex != ifindex)
			continue;
		if ((rt->rt6i_flags & (RTF_ROUTEINFO|RTF_GATEWAY)) != (RTF_ROUTEINFO|RTF_GATEWAY))
			continue;
		if (!ipv6_addr_equal(&rt->rt6i_gateway, gwaddr))
			continue;
		dst_hold(&rt->dst);
		break;
	}
out:
	read_unlock_bh(&table->tb6_lock);
	return rt;
}