static struct rt6_info *rt6_alloc_clone(struct rt6_info *ort,
					const struct in6_addr *daddr)
{
	struct rt6_info *rt = ip6_rt_copy(ort, daddr);

	if (rt)
		rt->rt6i_flags |= RTF_CACHE;
	return rt;
}