static void sit_route_add(struct net_device *dev)
{
	struct fib6_config cfg = {
		.fc_table = RT6_TABLE_MAIN,
		.fc_metric = IP6_RT_PRIO_ADDRCONF,
		.fc_ifindex = dev->ifindex,
		.fc_dst_len = 96,
		.fc_flags = RTF_UP | RTF_NONEXTHOP,
		.fc_nlinfo.nl_net = dev_net(dev),
	};

	/* prefix length - 96 bits "::d.d.d.d" */
	ip6_route_add(&cfg);
}