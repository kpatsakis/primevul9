static void addrconf_sit_config(struct net_device *dev)
{
	struct inet6_dev *idev;

	ASSERT_RTNL();

	/*
	 * Configure the tunnel with one of our IPv4
	 * addresses... we should configure all of
	 * our v4 addrs in the tunnel
	 */

	if ((idev = ipv6_find_idev(dev)) == NULL) {
		pr_debug("%s: add_dev failed\n", __func__);
		return;
	}

	if (dev->priv_flags & IFF_ISATAP) {
		struct in6_addr addr;

		ipv6_addr_set(&addr,  htonl(0xFE800000), 0, 0, 0);
		addrconf_prefix_route(&addr, 64, dev, 0, 0);
		if (!ipv6_generate_eui64(addr.s6_addr + 8, dev))
			addrconf_add_linklocal(idev, &addr);
		return;
	}

	sit_add_v4_addrs(idev);

	if (dev->flags&IFF_POINTOPOINT)
		addrconf_add_mroute(dev);
	else
		sit_route_add(dev);
}