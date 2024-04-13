static void addrconf_ip6_tnl_config(struct net_device *dev)
{
	struct inet6_dev *idev;

	ASSERT_RTNL();

	idev = addrconf_add_dev(dev);
	if (IS_ERR(idev)) {
		pr_debug("init ip6-ip6: add_dev failed\n");
		return;
	}
	ip6_tnl_add_linklocal(idev);
}