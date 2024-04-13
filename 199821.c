static void addrconf_dad_start(struct inet6_ifaddr *ifp)
{
	struct inet6_dev *idev = ifp->idev;
	struct net_device *dev = idev->dev;

	addrconf_join_solict(dev, &ifp->addr);

	net_srandom(ifp->addr.s6_addr32[3]);

	read_lock_bh(&idev->lock);
	spin_lock(&ifp->lock);
	if (ifp->state == INET6_IFADDR_STATE_DEAD)
		goto out;

	if (dev->flags&(IFF_NOARP|IFF_LOOPBACK) ||
	    idev->cnf.accept_dad < 1 ||
	    !(ifp->flags&IFA_F_TENTATIVE) ||
	    ifp->flags & IFA_F_NODAD) {
		ifp->flags &= ~(IFA_F_TENTATIVE|IFA_F_OPTIMISTIC|IFA_F_DADFAILED);
		spin_unlock(&ifp->lock);
		read_unlock_bh(&idev->lock);

		addrconf_dad_completed(ifp);
		return;
	}

	if (!(idev->if_flags & IF_READY)) {
		spin_unlock(&ifp->lock);
		read_unlock_bh(&idev->lock);
		/*
		 * If the device is not ready:
		 * - keep it tentative if it is a permanent address.
		 * - otherwise, kill it.
		 */
		in6_ifa_hold(ifp);
		addrconf_dad_stop(ifp, 0);
		return;
	}

	/*
	 * Optimistic nodes can start receiving
	 * Frames right away
	 */
	if (ifp->flags & IFA_F_OPTIMISTIC)
		ip6_ins_rt(ifp->rt);

	addrconf_dad_kick(ifp);
out:
	spin_unlock(&ifp->lock);
	read_unlock_bh(&idev->lock);
}