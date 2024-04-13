static struct rt6_info *addrconf_get_prefix_route(const struct in6_addr *pfx,
						  int plen,
						  const struct net_device *dev,
						  u32 flags, u32 noflags)
{
	struct fib6_node *fn;
	struct rt6_info *rt = NULL;
	struct fib6_table *table;

	table = fib6_get_table(dev_net(dev), RT6_TABLE_PREFIX);
	if (table == NULL)
		return NULL;

	read_lock_bh(&table->tb6_lock);
	fn = fib6_locate(&table->tb6_root, pfx, plen, NULL, 0);
	if (!fn)
		goto out;
	for (rt = fn->leaf; rt; rt = rt->dst.rt6_next) {
		if (rt->dst.dev->ifindex != dev->ifindex)
			continue;
		if ((rt->rt6i_flags & flags) != flags)
			continue;
		if ((rt->rt6i_flags & noflags) != 0)
			continue;
		dst_hold(&rt->dst);
		break;
	}
out:
	read_unlock_bh(&table->tb6_lock);
	return rt;
}