static struct dst_entry *ip6_route_redirect(struct net *net,
					const struct flowi6 *fl6,
					const struct in6_addr *gateway)
{
	int flags = RT6_LOOKUP_F_HAS_SADDR;
	struct ip6rd_flowi rdfl;

	rdfl.fl6 = *fl6;
	rdfl.gateway = *gateway;

	return fib6_rule_lookup(net, &rdfl.fl6,
				flags, __ip6_route_redirect);
}