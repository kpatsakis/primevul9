int ipv6_chk_addr(struct net *net, const struct in6_addr *addr,
		  const struct net_device *dev, int strict)
{
	struct inet6_ifaddr *ifp;
	unsigned int hash = inet6_addr_hash(addr);

	rcu_read_lock_bh();
	hlist_for_each_entry_rcu(ifp, &inet6_addr_lst[hash], addr_lst) {
		if (!net_eq(dev_net(ifp->idev->dev), net))
			continue;
		if (ipv6_addr_equal(&ifp->addr, addr) &&
		    !(ifp->flags&IFA_F_TENTATIVE) &&
		    (dev == NULL || ifp->idev->dev == dev ||
		     !(ifp->scope&(IFA_LINK|IFA_HOST) || strict))) {
			rcu_read_unlock_bh();
			return 1;
		}
	}

	rcu_read_unlock_bh();
	return 0;
}