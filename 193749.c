static struct rt6_info *ip6_pol_route_lookup(struct net *net,
					     struct fib6_table *table,
					     struct flowi6 *fl6, int flags)
{
	struct fib6_node *fn;
	struct rt6_info *rt;

	read_lock_bh(&table->tb6_lock);
	fn = fib6_lookup(&table->tb6_root, &fl6->daddr, &fl6->saddr);
restart:
	rt = fn->leaf;
	rt = rt6_device_match(net, rt, &fl6->saddr, fl6->flowi6_oif, flags);
	if (rt->rt6i_nsiblings && fl6->flowi6_oif == 0)
		rt = rt6_multipath_select(rt, fl6, fl6->flowi6_oif, flags);
	BACKTRACK(net, &fl6->saddr);
out:
	dst_use(&rt->dst, jiffies);
	read_unlock_bh(&table->tb6_lock);
	return rt;

}