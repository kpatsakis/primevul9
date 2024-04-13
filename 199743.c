static void addrconf_sysctl_register(struct inet6_dev *idev)
{
	neigh_sysctl_register(idev->dev, idev->nd_parms, "ipv6",
			      &ndisc_ifinfo_sysctl_change);
	__addrconf_sysctl_register(dev_net(idev->dev), idev->dev->name,
					idev, &idev->cnf);
}