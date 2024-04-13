struct rt6_info *rt6_get_dflt_router(const struct in6_addr *addr, struct net_device *dev)
{
	struct rt6_info *rt;
	struct fib6_table *table;

	table = fib6_get_table(dev_net(dev), RT6_TABLE_DFLT);
	if (!table)
		return NULL;

	read_lock_bh(&table->tb6_lock);
	for (rt = table->tb6_root.leaf; rt; rt = rt->dst.rt6_next) {
		if (dev == rt->dst.dev &&
		    ((rt->rt6i_flags & (RTF_ADDRCONF | RTF_DEFAULT)) == (RTF_ADDRCONF | RTF_DEFAULT)) &&
		    ipv6_addr_equal(&rt->rt6i_gateway, addr))
			break;
	}
	if (rt)
		dst_hold(&rt->dst);
	read_unlock_bh(&table->tb6_lock);
	return rt;
}