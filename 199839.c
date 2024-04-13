static void addrconf_dev_config(struct net_device *dev)
{
	struct in6_addr addr;
	struct inet6_dev *idev;

	ASSERT_RTNL();

	if ((dev->type != ARPHRD_ETHER) &&
	    (dev->type != ARPHRD_FDDI) &&
	    (dev->type != ARPHRD_ARCNET) &&
	    (dev->type != ARPHRD_INFINIBAND) &&
	    (dev->type != ARPHRD_IEEE802154) &&
	    (dev->type != ARPHRD_IEEE1394)) {
		/* Alas, we support only Ethernet autoconfiguration. */
		return;
	}

	idev = addrconf_add_dev(dev);
	if (IS_ERR(idev))
		return;

	memset(&addr, 0, sizeof(struct in6_addr));
	addr.s6_addr32[0] = htonl(0xFE800000);

	if (ipv6_generate_eui64(addr.s6_addr + 8, dev) == 0)
		addrconf_add_linklocal(idev, &addr);
}