static void add_addr(struct inet6_dev *idev, const struct in6_addr *addr,
		     int plen, int scope)
{
	struct inet6_ifaddr *ifp;

	ifp = ipv6_add_addr(idev, addr, NULL, plen,
			    scope, IFA_F_PERMANENT, 0, 0);
	if (!IS_ERR(ifp)) {
		spin_lock_bh(&ifp->lock);
		ifp->flags &= ~IFA_F_TENTATIVE;
		spin_unlock_bh(&ifp->lock);
		ipv6_ifa_notify(RTM_NEWADDR, ifp);
		in6_ifa_put(ifp);
	}
}