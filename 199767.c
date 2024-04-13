static int inet6_set_iftoken(struct inet6_dev *idev, struct in6_addr *token)
{
	struct inet6_ifaddr *ifp;
	struct net_device *dev = idev->dev;
	bool update_rs = false;
	struct in6_addr ll_addr;

	if (token == NULL)
		return -EINVAL;
	if (ipv6_addr_any(token))
		return -EINVAL;
	if (dev->flags & (IFF_LOOPBACK | IFF_NOARP))
		return -EINVAL;
	if (!ipv6_accept_ra(idev))
		return -EINVAL;
	if (idev->cnf.rtr_solicits <= 0)
		return -EINVAL;

	write_lock_bh(&idev->lock);

	BUILD_BUG_ON(sizeof(token->s6_addr) != 16);
	memcpy(idev->token.s6_addr + 8, token->s6_addr + 8, 8);

	write_unlock_bh(&idev->lock);

	if (!idev->dead && (idev->if_flags & IF_READY) &&
	    !ipv6_get_lladdr(dev, &ll_addr, IFA_F_TENTATIVE |
			     IFA_F_OPTIMISTIC)) {

		/* If we're not ready, then normal ifup will take care
		 * of this. Otherwise, we need to request our rs here.
		 */
		ndisc_send_rs(dev, &ll_addr, &in6addr_linklocal_allrouters);
		update_rs = true;
	}

	write_lock_bh(&idev->lock);

	if (update_rs) {
		idev->if_flags |= IF_RS_SENT;
		idev->rs_probes = 1;
		addrconf_mod_rs_timer(idev, idev->cnf.rtr_solicit_interval);
	}

	/* Well, that's kinda nasty ... */
	list_for_each_entry(ifp, &idev->addr_list, if_list) {
		spin_lock(&ifp->lock);
		if (ifp->tokenized) {
			ifp->valid_lft = 0;
			ifp->prefered_lft = 0;
		}
		spin_unlock(&ifp->lock);
	}

	write_unlock_bh(&idev->lock);
	addrconf_verify(0);
	return 0;
}