static void addrconf_dad_completed(struct inet6_ifaddr *ifp)
{
	struct net_device *dev = ifp->idev->dev;
	struct in6_addr lladdr;
	bool send_rs, send_mld;

	addrconf_del_dad_timer(ifp);

	/*
	 *	Configure the address for reception. Now it is valid.
	 */

	ipv6_ifa_notify(RTM_NEWADDR, ifp);

	/* If added prefix is link local and we are prepared to process
	   router advertisements, start sending router solicitations.
	 */

	read_lock_bh(&ifp->idev->lock);
	spin_lock(&ifp->lock);
	send_mld = ipv6_addr_type(&ifp->addr) & IPV6_ADDR_LINKLOCAL &&
		   ifp->idev->valid_ll_addr_cnt == 1;
	send_rs = send_mld &&
		  ipv6_accept_ra(ifp->idev) &&
		  ifp->idev->cnf.rtr_solicits > 0 &&
		  (dev->flags&IFF_LOOPBACK) == 0;
	spin_unlock(&ifp->lock);
	read_unlock_bh(&ifp->idev->lock);

	/* While dad is in progress mld report's source address is in6_addrany.
	 * Resend with proper ll now.
	 */
	if (send_mld)
		ipv6_mc_dad_complete(ifp->idev);

	if (send_rs) {
		/*
		 *	If a host as already performed a random delay
		 *	[...] as part of DAD [...] there is no need
		 *	to delay again before sending the first RS
		 */
		if (ipv6_get_lladdr(dev, &lladdr, IFA_F_TENTATIVE))
			return;
		ndisc_send_rs(dev, &lladdr, &in6addr_linklocal_allrouters);

		write_lock_bh(&ifp->idev->lock);
		spin_lock(&ifp->lock);
		ifp->idev->rs_probes = 1;
		ifp->idev->if_flags |= IF_RS_SENT;
		addrconf_mod_rs_timer(ifp->idev,
				      ifp->idev->cnf.rtr_solicit_interval);
		spin_unlock(&ifp->lock);
		write_unlock_bh(&ifp->idev->lock);
	}
}