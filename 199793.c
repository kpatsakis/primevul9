addrconf_prefix_route(struct in6_addr *pfx, int plen, struct net_device *dev,
		      unsigned long expires, u32 flags)
{
	struct fib6_config cfg = {
		.fc_table = RT6_TABLE_PREFIX,
		.fc_metric = IP6_RT_PRIO_ADDRCONF,
		.fc_ifindex = dev->ifindex,
		.fc_expires = expires,
		.fc_dst_len = plen,
		.fc_flags = RTF_UP | flags,
		.fc_nlinfo.nl_net = dev_net(dev),
		.fc_protocol = RTPROT_KERNEL,
	};

	cfg.fc_dst = *pfx;

	/* Prevent useless cloning on PtP SIT.
	   This thing is done here expecting that the whole
	   class of non-broadcast devices need not cloning.
	 */
#if IS_ENABLED(CONFIG_IPV6_SIT)
	if (dev->type == ARPHRD_SIT && (dev->flags & IFF_POINTOPOINT))
		cfg.fc_flags |= RTF_NONEXTHOP;
#endif

	ip6_route_add(&cfg);
}