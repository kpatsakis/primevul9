static void addrconf_add_mroute(struct net_device *dev)
{
	struct fib6_config cfg = {
		.fc_table = RT6_TABLE_LOCAL,
		.fc_metric = IP6_RT_PRIO_ADDRCONF,
		.fc_ifindex = dev->ifindex,
		.fc_dst_len = 8,
		.fc_flags = RTF_UP,
		.fc_nlinfo.nl_net = dev_net(dev),
	};

	ipv6_addr_set(&cfg.fc_dst, htonl(0xFF000000), 0, 0, 0);

	ip6_route_add(&cfg);
}