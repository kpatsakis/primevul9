static int ipv6_create_tempaddr(struct inet6_ifaddr *ifp, struct inet6_ifaddr *ift)
{
	struct inet6_dev *idev = ifp->idev;
	struct in6_addr addr, *tmpaddr;
	unsigned long tmp_prefered_lft, tmp_valid_lft, tmp_tstamp, age;
	unsigned long regen_advance;
	int tmp_plen;
	int ret = 0;
	int max_addresses;
	u32 addr_flags;
	unsigned long now = jiffies;

	write_lock(&idev->lock);
	if (ift) {
		spin_lock_bh(&ift->lock);
		memcpy(&addr.s6_addr[8], &ift->addr.s6_addr[8], 8);
		spin_unlock_bh(&ift->lock);
		tmpaddr = &addr;
	} else {
		tmpaddr = NULL;
	}
retry:
	in6_dev_hold(idev);
	if (idev->cnf.use_tempaddr <= 0) {
		write_unlock(&idev->lock);
		pr_info("%s: use_tempaddr is disabled\n", __func__);
		in6_dev_put(idev);
		ret = -1;
		goto out;
	}
	spin_lock_bh(&ifp->lock);
	if (ifp->regen_count++ >= idev->cnf.regen_max_retry) {
		idev->cnf.use_tempaddr = -1;	/*XXX*/
		spin_unlock_bh(&ifp->lock);
		write_unlock(&idev->lock);
		pr_warn("%s: regeneration time exceeded - disabled temporary address support\n",
			__func__);
		in6_dev_put(idev);
		ret = -1;
		goto out;
	}
	in6_ifa_hold(ifp);
	memcpy(addr.s6_addr, ifp->addr.s6_addr, 8);
	__ipv6_try_regen_rndid(idev, tmpaddr);
	memcpy(&addr.s6_addr[8], idev->rndid, 8);
	age = (now - ifp->tstamp) / HZ;
	tmp_valid_lft = min_t(__u32,
			      ifp->valid_lft,
			      idev->cnf.temp_valid_lft + age);
	tmp_prefered_lft = min_t(__u32,
				 ifp->prefered_lft,
				 idev->cnf.temp_prefered_lft + age -
				 idev->cnf.max_desync_factor);
	tmp_plen = ifp->prefix_len;
	max_addresses = idev->cnf.max_addresses;
	tmp_tstamp = ifp->tstamp;
	spin_unlock_bh(&ifp->lock);

	regen_advance = idev->cnf.regen_max_retry *
	                idev->cnf.dad_transmits *
	                idev->nd_parms->retrans_time / HZ;
	write_unlock(&idev->lock);

	/* A temporary address is created only if this calculated Preferred
	 * Lifetime is greater than REGEN_ADVANCE time units.  In particular,
	 * an implementation must not create a temporary address with a zero
	 * Preferred Lifetime.
	 */
	if (tmp_prefered_lft <= regen_advance) {
		in6_ifa_put(ifp);
		in6_dev_put(idev);
		ret = -1;
		goto out;
	}

	addr_flags = IFA_F_TEMPORARY;
	/* set in addrconf_prefix_rcv() */
	if (ifp->flags & IFA_F_OPTIMISTIC)
		addr_flags |= IFA_F_OPTIMISTIC;

	ift = ipv6_add_addr(idev, &addr, NULL, tmp_plen,
			    ipv6_addr_scope(&addr), addr_flags,
			    tmp_valid_lft, tmp_prefered_lft);
	if (IS_ERR(ift)) {
		in6_ifa_put(ifp);
		in6_dev_put(idev);
		pr_info("%s: retry temporary address regeneration\n", __func__);
		tmpaddr = &addr;
		write_lock(&idev->lock);
		goto retry;
	}

	spin_lock_bh(&ift->lock);
	ift->ifpub = ifp;
	ift->cstamp = now;
	ift->tstamp = tmp_tstamp;
	spin_unlock_bh(&ift->lock);

	addrconf_dad_start(ift);
	in6_ifa_put(ift);
	in6_dev_put(idev);
out:
	return ret;
}