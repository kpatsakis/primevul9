struct dst_entry *ip6_route_lookup(struct net *net, struct flowi6 *fl6,
				    int flags)
{
	return fib6_rule_lookup(net, fl6, flags, ip6_pol_route_lookup);
}