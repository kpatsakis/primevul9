struct inet6_ifaddr *ipv6_get_ifaddr(struct net *net, const struct in6_addr *addr,
				     struct net_device *dev, int strict)
{
	struct inet6_ifaddr *ifp, *result = NULL;
	unsigned int hash = inet6_addr_hash(addr);

	rcu_read_lock_bh();
	hlist_for_each_entry_rcu_bh(ifp, &inet6_addr_lst[hash], addr_lst) {
		if (!net_eq(dev_net(ifp->idev->dev), net))
			continue;
		if (ipv6_addr_equal(&ifp->addr, addr)) {
			if (dev == NULL || ifp->idev->dev == dev ||
			    !(ifp->scope&(IFA_LINK|IFA_HOST) || strict)) {
				result = ifp;
				in6_ifa_hold(ifp);
				break;
			}
		}
	}
	rcu_read_unlock_bh();

	return result;
}