static void addrconf_rs_timer(unsigned long data)
{
	struct inet6_dev *idev = (struct inet6_dev *)data;
	struct in6_addr lladdr;

	write_lock(&idev->lock);
	if (idev->dead || !(idev->if_flags & IF_READY))
		goto out;

	if (!ipv6_accept_ra(idev))
		goto out;

	/* Announcement received after solicitation was sent */
	if (idev->if_flags & IF_RA_RCVD)
		goto out;

	if (idev->rs_probes++ < idev->cnf.rtr_solicits) {
		if (!__ipv6_get_lladdr(idev, &lladdr, IFA_F_TENTATIVE))
			ndisc_send_rs(idev->dev, &lladdr,
				      &in6addr_linklocal_allrouters);
		else
			goto out;

		/* The wait after the last probe can be shorter */
		addrconf_mod_rs_timer(idev, (idev->rs_probes ==
					     idev->cnf.rtr_solicits) ?
				      idev->cnf.rtr_solicit_delay :
				      idev->cnf.rtr_solicit_interval);
	} else {
		/*
		 * Note: we do not support deprecated "all on-link"
		 * assumption any longer.
		 */
		pr_debug("%s: no IPv6 routers present\n", idev->dev->name);
	}

out:
	write_unlock(&idev->lock);
	in6_dev_put(idev);
}