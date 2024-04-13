static void addrconf_sysctl_unregister(struct inet6_dev *idev)
{
	__addrconf_sysctl_unregister(&idev->cnf);
	neigh_sysctl_unregister(idev->nd_parms);
}