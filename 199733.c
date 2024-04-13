static bool ipv6_chk_same_addr(struct net *net, const struct in6_addr *addr,
			       struct net_device *dev)
{
	unsigned int hash = inet6_addr_hash(addr);
	struct inet6_ifaddr *ifp;

	hlist_for_each_entry(ifp, &inet6_addr_lst[hash], addr_lst) {
		if (!net_eq(dev_net(ifp->idev->dev), net))
			continue;
		if (ipv6_addr_equal(&ifp->addr, addr)) {
			if (dev == NULL || ifp->idev->dev == dev)
				return true;
		}
	}
	return false;
}