static void addrconf_disable_change(struct net *net, __s32 newf)
{
	struct net_device *dev;
	struct inet6_dev *idev;

	rcu_read_lock();
	for_each_netdev_rcu(net, dev) {
		idev = __in6_dev_get(dev);
		if (idev) {
			int changed = (!idev->cnf.disable_ipv6) ^ (!newf);
			idev->cnf.disable_ipv6 = newf;
			if (changed)
				dev_disable_change(idev);
		}
	}
	rcu_read_unlock();
}