void rt6_purge_dflt_routers(struct net *net)
{
	struct rt6_info *rt;
	struct fib6_table *table;

	/* NOTE: Keep consistent with rt6_get_dflt_router */
	table = fib6_get_table(net, RT6_TABLE_DFLT);
	if (!table)
		return;

restart:
	read_lock_bh(&table->tb6_lock);
	for (rt = table->tb6_root.leaf; rt; rt = rt->dst.rt6_next) {
		if (rt->rt6i_flags & (RTF_DEFAULT | RTF_ADDRCONF) &&
		    (!rt->rt6i_idev || rt->rt6i_idev->cnf.accept_ra != 2)) {
			dst_hold(&rt->dst);
			read_unlock_bh(&table->tb6_lock);
			ip6_del_rt(rt);
			goto restart;
		}
	}
	read_unlock_bh(&table->tb6_lock);
}