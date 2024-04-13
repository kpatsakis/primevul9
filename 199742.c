static void addrconf_forward_change(struct net *net, __s32 newf)
{
	struct net_device *dev;
	struct inet6_dev *idev;

	for_each_netdev(net, dev) {
		idev = __in6_dev_get(dev);
		if (idev) {
			int changed = (!idev->cnf.forwarding) ^ (!newf);
			idev->cnf.forwarding = newf;
			if (changed)
				dev_forward_change(idev);
		}
	}
}