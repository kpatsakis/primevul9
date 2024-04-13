static void dev_disable_change(struct inet6_dev *idev)
{
	struct netdev_notifier_info info;

	if (!idev || !idev->dev)
		return;

	netdev_notifier_info_init(&info, idev->dev);
	if (idev->cnf.disable_ipv6)
		addrconf_notify(NULL, NETDEV_DOWN, &info);
	else
		addrconf_notify(NULL, NETDEV_UP, &info);
}