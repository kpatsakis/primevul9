static int inet6_addr_modify(struct inet6_ifaddr *ifp, u8 ifa_flags,
			     u32 prefered_lft, u32 valid_lft)
{
	u32 flags;
	clock_t expires;
	unsigned long timeout;

	if (!valid_lft || (prefered_lft > valid_lft))
		return -EINVAL;

	timeout = addrconf_timeout_fixup(valid_lft, HZ);
	if (addrconf_finite_timeout(timeout)) {
		expires = jiffies_to_clock_t(timeout * HZ);
		valid_lft = timeout;
		flags = RTF_EXPIRES;
	} else {
		expires = 0;
		flags = 0;
		ifa_flags |= IFA_F_PERMANENT;
	}

	timeout = addrconf_timeout_fixup(prefered_lft, HZ);
	if (addrconf_finite_timeout(timeout)) {
		if (timeout == 0)
			ifa_flags |= IFA_F_DEPRECATED;
		prefered_lft = timeout;
	}

	spin_lock_bh(&ifp->lock);
	ifp->flags = (ifp->flags & ~(IFA_F_DEPRECATED | IFA_F_PERMANENT | IFA_F_NODAD | IFA_F_HOMEADDRESS)) | ifa_flags;
	ifp->tstamp = jiffies;
	ifp->valid_lft = valid_lft;
	ifp->prefered_lft = prefered_lft;

	spin_unlock_bh(&ifp->lock);
	if (!(ifp->flags&IFA_F_TENTATIVE))
		ipv6_ifa_notify(0, ifp);

	addrconf_prefix_route(&ifp->addr, ifp->prefix_len, ifp->idev->dev,
			      expires, flags);
	addrconf_verify(0);

	return 0;
}