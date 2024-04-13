static int inet6_addr_add(struct net *net, int ifindex, const struct in6_addr *pfx,
			  const struct in6_addr *peer_pfx,
			  unsigned int plen, __u8 ifa_flags, __u32 prefered_lft,
			  __u32 valid_lft)
{
	struct inet6_ifaddr *ifp;
	struct inet6_dev *idev;
	struct net_device *dev;
	int scope;
	u32 flags;
	clock_t expires;
	unsigned long timeout;

	ASSERT_RTNL();

	if (plen > 128)
		return -EINVAL;

	/* check the lifetime */
	if (!valid_lft || prefered_lft > valid_lft)
		return -EINVAL;

	dev = __dev_get_by_index(net, ifindex);
	if (!dev)
		return -ENODEV;

	idev = addrconf_add_dev(dev);
	if (IS_ERR(idev))
		return PTR_ERR(idev);

	scope = ipv6_addr_scope(pfx);

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

	ifp = ipv6_add_addr(idev, pfx, peer_pfx, plen, scope, ifa_flags,
			    valid_lft, prefered_lft);

	if (!IS_ERR(ifp)) {
		addrconf_prefix_route(&ifp->addr, ifp->prefix_len, dev,
				      expires, flags);
		/*
		 * Note that section 3.1 of RFC 4429 indicates
		 * that the Optimistic flag should not be set for
		 * manually configured addresses
		 */
		addrconf_dad_start(ifp);
		in6_ifa_put(ifp);
		addrconf_verify(0);
		return 0;
	}

	return PTR_ERR(ifp);
}