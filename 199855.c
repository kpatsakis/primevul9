static int inet6_addr_del(struct net *net, int ifindex, const struct in6_addr *pfx,
			  unsigned int plen)
{
	struct inet6_ifaddr *ifp;
	struct inet6_dev *idev;
	struct net_device *dev;

	if (plen > 128)
		return -EINVAL;

	dev = __dev_get_by_index(net, ifindex);
	if (!dev)
		return -ENODEV;

	if ((idev = __in6_dev_get(dev)) == NULL)
		return -ENXIO;

	read_lock_bh(&idev->lock);
	list_for_each_entry(ifp, &idev->addr_list, if_list) {
		if (ifp->prefix_len == plen &&
		    ipv6_addr_equal(pfx, &ifp->addr)) {
			in6_ifa_hold(ifp);
			read_unlock_bh(&idev->lock);

			ipv6_del_addr(ifp);
			return 0;
		}
	}
	read_unlock_bh(&idev->lock);
	return -EADDRNOTAVAIL;
}