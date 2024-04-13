static struct inet6_dev *ipv6_find_idev(struct net_device *dev)
{
	struct inet6_dev *idev;

	ASSERT_RTNL();

	idev = __in6_dev_get(dev);
	if (!idev) {
		idev = ipv6_add_dev(dev);
		if (!idev)
			return NULL;
	}

	if (dev->flags&IFF_UP)
		ipv6_mc_up(idev);
	return idev;
}