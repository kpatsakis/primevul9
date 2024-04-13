static struct rt6_info *ip6_pol_route(struct net *net, struct fib6_table *table, int oif,
				      struct flowi6 *fl6, int flags)
{
	struct fib6_node *fn, *saved_fn;
	struct rt6_info *rt, *nrt;
	int strict = 0;
	int attempts = 3;
	int err;

	strict |= flags & RT6_LOOKUP_F_IFACE;
	if (net->ipv6.devconf_all->forwarding == 0)
		strict |= RT6_LOOKUP_F_REACHABLE;

redo_fib6_lookup_lock:
	read_lock_bh(&table->tb6_lock);

	fn = fib6_lookup(&table->tb6_root, &fl6->daddr, &fl6->saddr);
	saved_fn = fn;

redo_rt6_select:
	rt = rt6_select(fn, oif, strict);
	if (rt->rt6i_nsiblings)
		rt = rt6_multipath_select(rt, fl6, oif, strict);
	if (rt == net->ipv6.ip6_null_entry) {
		fn = fib6_backtrack(fn, &fl6->saddr);
		if (fn)
			goto redo_rt6_select;
		else if (strict & RT6_LOOKUP_F_REACHABLE) {
			/* also consider unreachable route */
			strict &= ~RT6_LOOKUP_F_REACHABLE;
			fn = saved_fn;
			goto redo_rt6_select;
		} else {
			dst_hold(&rt->dst);
			read_unlock_bh(&table->tb6_lock);
			goto out2;
		}
	}

	dst_hold(&rt->dst);
	read_unlock_bh(&table->tb6_lock);

	if (rt->rt6i_flags & RTF_CACHE)
		goto out2;

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
	goto redo_fib6_lookup_lock;

out2:
	rt->dst.lastuse = jiffies;
	rt->dst.__use++;

	return rt;
}