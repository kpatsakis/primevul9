static void addrconf_dad_timer(unsigned long data)
{
	struct inet6_ifaddr *ifp = (struct inet6_ifaddr *) data;
	struct inet6_dev *idev = ifp->idev;
	struct in6_addr mcaddr;

	if (!ifp->dad_probes && addrconf_dad_end(ifp))
		goto out;

	write_lock(&idev->lock);
	if (idev->dead || !(idev->if_flags & IF_READY)) {
		write_unlock(&idev->lock);
		goto out;
	}

	spin_lock(&ifp->lock);
	if (ifp->state == INET6_IFADDR_STATE_DEAD) {
		spin_unlock(&ifp->lock);
		write_unlock(&idev->lock);
		goto out;
	}

	if (ifp->dad_probes == 0) {
		/*
		 * DAD was successful
		 */

		ifp->flags &= ~(IFA_F_TENTATIVE|IFA_F_OPTIMISTIC|IFA_F_DADFAILED);
		spin_unlock(&ifp->lock);
		write_unlock(&idev->lock);

		addrconf_dad_completed(ifp);

		goto out;
	}

	ifp->dad_probes--;
	addrconf_mod_dad_timer(ifp, ifp->idev->nd_parms->retrans_time);
	spin_unlock(&ifp->lock);
	write_unlock(&idev->lock);

	/* send a neighbour solicitation for our addr */
	addrconf_addr_solict_mult(&ifp->addr, &mcaddr);
	ndisc_send_ns(ifp->idev->dev, NULL, &ifp->addr, &mcaddr, &in6addr_any);
out:
	in6_ifa_put(ifp);
}