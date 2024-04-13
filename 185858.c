static struct rt6_info *rt6_alloc_cow(struct rt6_info *ort,
				      const struct in6_addr *daddr,
				      const struct in6_addr *saddr)
{
	struct rt6_info *rt;

	/*
	 *	Clone the route.
	 */

	rt = ip6_rt_copy(ort, daddr);

	if (rt) {
		if (ort->rt6i_dst.plen != 128 &&
		    ipv6_addr_equal(&ort->rt6i_dst.addr, daddr))
			rt->rt6i_flags |= RTF_ANYCAST;

		rt->rt6i_flags |= RTF_CACHE;

#ifdef CONFIG_IPV6_SUBTREES
		if (rt->rt6i_src.plen && saddr) {
			rt->rt6i_src.addr = *saddr;
			rt->rt6i_src.plen = 128;
		}
#endif
	}

	return rt;
}