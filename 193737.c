static struct rt6_info *ip6_pol_route(struct net *net, struct fib6_table *table, int oif,
				      struct flowi6 *fl6, int flags)
{
	struct fib6_node *fn;
	struct rt6_info *rt, *nrt;
	int strict = 0;
	int attempts = 3;
	int err;
	int reachable = net->ipv6.devconf_all->forwarding ? 0 : RT6_LOOKUP_F_REACHABLE;

	strict |= flags & RT6_LOOKUP_F_IFACE;

relookup:
	read_lock_bh(&table->tb6_lock);

restart_2:
	fn = fib6_lookup(&table->tb6_root, &fl6->daddr, &fl6->saddr);

restart:
	rt = rt6_select(fn, oif, strict | reachable);
	if (rt->rt6i_nsiblings)
		rt = rt6_multipath_select(rt, fl6, oif, strict | reachable);
	BACKTRACK(net, &fl6->saddr);
	if (rt == net->ipv6.ip6_null_entry ||
	    rt->rt6i_flags & RTF_CACHE)
		goto out;

	dst_hold(&rt->dst);
	read_unlock_bh(&table->tb6_lock);

	if (!(rt->rt6i_flags & (RTF_NONEXTHOP | RTF_GATEWAY)))
		nrt = rt6_alloc_cow(rt, &fl6->daddr, &fl6->saddr);
	else if (!(rt->dst.flags & DST_HOST))
		nrt = rt6_alloc_clone(rt, &fl6->daddr);
	else
		goto out2;

	ip6_rt_put(rt);
	rt = nrt ? : net->ipv6.ip6_null_entry;

	dst_hold(&rt->dst);
	if (nrt) {
		err = ip6_ins_rt(nrt);
		if (!err)
			goto out2;
	}

	if (--attempts <= 0)
		goto out2;

	/*
	 * Race condition! In the gap, when table->tb6_lock was
	 * released someone could insert this route.  Relookup.
	 */
	ip6_rt_put(rt);
	goto relookup;

out:
	if (reachable) {
		reachable = 0;
		goto restart_2;
	}
	dst_hold(&rt->dst);
	read_unlock_bh(&table->tb6_lock);
out2:
	rt->dst.lastuse = jiffies;
	rt->dst.__use++;

	return rt;
}