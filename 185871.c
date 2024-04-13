struct rt6_info *addrconf_dst_alloc(struct inet6_dev *idev,
				    const struct in6_addr *addr,
				    bool anycast)
{
	struct net *net = dev_net(idev->dev);
	struct rt6_info *rt = ip6_dst_alloc(net, net->loopback_dev,
					    DST_NOCOUNT, NULL);
	if (!rt)
		return ERR_PTR(-ENOMEM);

	in6_dev_hold(idev);

	rt->dst.flags |= DST_HOST;
	rt->dst.input = ip6_input;
	rt->dst.output = ip6_output;
	rt->rt6i_idev = idev;

	rt->rt6i_flags = RTF_UP | RTF_NONEXTHOP;
	if (anycast)
		rt->rt6i_flags |= RTF_ANYCAST;
	else
		rt->rt6i_flags |= RTF_LOCAL;

	rt->rt6i_gateway  = *addr;
	rt->rt6i_dst.addr = *addr;
	rt->rt6i_dst.plen = 128;
	rt->rt6i_table = fib6_get_table(net, RT6_TABLE_LOCAL);

	atomic_set(&rt->dst.__refcnt, 1);

	return rt;
}