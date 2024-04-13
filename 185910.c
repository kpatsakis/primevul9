static struct rt6_info *ip6_rt_copy(struct rt6_info *ort,
				    const struct in6_addr *dest)
{
	struct net *net = dev_net(ort->dst.dev);
	struct rt6_info *rt = ip6_dst_alloc(net, ort->dst.dev, 0,
					    ort->rt6i_table);

	if (rt) {
		rt->dst.input = ort->dst.input;
		rt->dst.output = ort->dst.output;
		rt->dst.flags |= DST_HOST;

		rt->rt6i_dst.addr = *dest;
		rt->rt6i_dst.plen = 128;
		dst_copy_metrics(&rt->dst, &ort->dst);
		rt->dst.error = ort->dst.error;
		rt->rt6i_idev = ort->rt6i_idev;
		if (rt->rt6i_idev)
			in6_dev_hold(rt->rt6i_idev);
		rt->dst.lastuse = jiffies;

		if (ort->rt6i_flags & RTF_GATEWAY)
			rt->rt6i_gateway = ort->rt6i_gateway;
		else
			rt->rt6i_gateway = *dest;
		rt->rt6i_flags = ort->rt6i_flags;
		rt6_set_from(rt, ort);
		rt->rt6i_metric = 0;

#ifdef CONFIG_IPV6_SUBTREES
		memcpy(&rt->rt6i_src, &ort->rt6i_src, sizeof(struct rt6key));
#endif
		memcpy(&rt->rt6i_prefsrc, &ort->rt6i_prefsrc, sizeof(struct rt6key));
		rt->rt6i_table = ort->rt6i_table;
	}
	return rt;
}