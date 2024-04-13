static int fib6_clean_tohost(struct rt6_info *rt, void *arg)
{
	struct in6_addr *gateway = (struct in6_addr *)arg;

	if ((((rt->rt6i_flags & RTF_RA_ROUTER) == RTF_RA_ROUTER) ||
	     ((rt->rt6i_flags & RTF_CACHE_GATEWAY) == RTF_CACHE_GATEWAY)) &&
	     ipv6_addr_equal(gateway, &rt->rt6i_gateway)) {
		return -1;
	}
	return 0;
}