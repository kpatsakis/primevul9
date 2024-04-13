static struct inet6_dev *addrconf_add_dev(struct net_device *dev)
{
	struct inet6_dev *idev;

	ASSERT_RTNL();

	idev = ipv6_find_idev(dev);
	if (!idev)
		return ERR_PTR(-ENOBUFS);

	if (idev->cnf.disable_ipv6)
		return ERR_PTR(-EACCES);

	/* Add default multicast route */
	if (!(dev->flags & IFF_LOOPBACK))
		addrconf_add_mroute(dev);

	return idev;
}