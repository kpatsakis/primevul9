static void addrconf_gre_config(struct net_device *dev)
{
	struct inet6_dev *idev;
	struct in6_addr addr;

	ASSERT_RTNL();

	if ((idev = ipv6_find_idev(dev)) == NULL) {
		pr_debug("%s: add_dev failed\n", __func__);
		return;
	}

	ipv6_addr_set(&addr,  htonl(0xFE800000), 0, 0, 0);
	addrconf_prefix_route(&addr, 64, dev, 0, 0);

	if (!ipv6_generate_eui64(addr.s6_addr + 8, dev))
		addrconf_add_linklocal(idev, &addr);
}