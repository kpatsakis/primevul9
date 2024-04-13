static int rt6_score_route(struct rt6_info *rt, int oif,
			   int strict)
{
	int m;

	m = rt6_check_dev(rt, oif);
	if (!m && (strict & RT6_LOOKUP_F_IFACE))
		return RT6_NUD_FAIL_HARD;
#ifdef CONFIG_IPV6_ROUTER_PREF
	m |= IPV6_DECODE_PREF(IPV6_EXTRACT_PREF(rt->rt6i_flags)) << 2;
#endif
	if (strict & RT6_LOOKUP_F_REACHABLE) {
		int n = rt6_check_neigh(rt);
		if (n < 0)
			return n;
	}
	return m;
}