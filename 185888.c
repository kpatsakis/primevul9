static struct rt6_info *__ip6_route_redirect(struct net *net,
					     struct fib6_table *table,
					     struct flowi6 *fl6,
					     int flags)
{
	struct ip6rd_flowi *rdfl = (struct ip6rd_flowi *)fl6;
	struct rt6_info *rt;
	struct fib6_node *fn;

	/* Get the "current" route for this destination and
	 * check if the redirect has come from approriate router.
	 *
	 * RFC 4861 specifies that redirects should only be
	 * accepted if they come from the nexthop to the target.
	 * Due to the way the routes are chosen, this notion
	 * is a bit fuzzy and one might need to check all possible
	 * routes.
	 */

	read_lock_bh(&table->tb6_lock);
	fn = fib6_lookup(&table->tb6_root, &fl6->daddr, &fl6->saddr);
restart:
	for (rt = fn->leaf; rt; rt = rt->dst.rt6_next) {
		if (rt6_check_expired(rt))
			continue;
		if (rt->dst.error)
			break;
		if (!(rt->rt6i_flags & RTF_GATEWAY))
			continue;
		if (fl6->flowi6_oif != rt->dst.dev->ifindex)
			continue;
		if (!ipv6_addr_equal(&rdfl->gateway, &rt->rt6i_gateway))
			continue;
		break;
	}

	if (!rt)
		rt = net->ipv6.ip6_null_entry;
	else if (rt->dst.error) {
		rt = net->ipv6.ip6_null_entry;
	} else if (rt == net->ipv6.ip6_null_entry) {
		fn = fib6_backtrack(fn, &fl6->saddr);
		if (fn)
			goto restart;
	}

	dst_hold(&rt->dst);

	read_unlock_bh(&table->tb6_lock);

	return rt;
};