static void addrconf_type_change(struct net_device *dev, unsigned long event)
{
	struct inet6_dev *idev;
	ASSERT_RTNL();

	idev = __in6_dev_get(dev);

	if (event == NETDEV_POST_TYPE_CHANGE)
		ipv6_mc_remap(idev);
	else if (event == NETDEV_PRE_TYPE_CHANGE)
		ipv6_mc_unmap(idev);
}