static void addrconf_dad_stop(struct inet6_ifaddr *ifp, int dad_failed)
{
	if (ifp->flags&IFA_F_PERMANENT) {
		spin_lock_bh(&ifp->lock);
		addrconf_del_dad_timer(ifp);
		ifp->flags |= IFA_F_TENTATIVE;
		if (dad_failed)
			ifp->flags |= IFA_F_DADFAILED;
		spin_unlock_bh(&ifp->lock);
		if (dad_failed)
			ipv6_ifa_notify(0, ifp);
		in6_ifa_put(ifp);
#ifdef CONFIG_IPV6_PRIVACY
	} else if (ifp->flags&IFA_F_TEMPORARY) {
		struct inet6_ifaddr *ifpub;
		spin_lock_bh(&ifp->lock);
		ifpub = ifp->ifpub;
		if (ifpub) {
			in6_ifa_hold(ifpub);
			spin_unlock_bh(&ifp->lock);
			ipv6_create_tempaddr(ifpub, ifp);
			in6_ifa_put(ifpub);
		} else {
			spin_unlock_bh(&ifp->lock);
		}
		ipv6_del_addr(ifp);
#endif
	} else
		ipv6_del_addr(ifp);
}