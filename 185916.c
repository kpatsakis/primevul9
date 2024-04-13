struct dst_entry *ip6_route_output(struct net *net, const struct sock *sk,
				    struct flowi6 *fl6)
{
	int flags = 0;

	fl6->flowi6_iif = LOOPBACK_IFINDEX;

	if ((sk && sk->sk_bound_dev_if) || rt6_need_strict(&fl6->daddr))
		flags |= RT6_LOOKUP_F_IFACE;

	if (!ipv6_addr_any(&fl6->saddr))
		flags |= RT6_LOOKUP_F_HAS_SADDR;
	else if (sk)
		flags |= rt6_srcprefs2flags(inet6_sk(sk)->srcprefs);

	return fib6_rule_lookup(net, fl6, flags, ip6_pol_route_output);
}