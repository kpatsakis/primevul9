void addrconf_prefix_rcv(struct net_device *dev, u8 *opt, int len, bool sllao)
{
	struct prefix_info *pinfo;
	__u32 valid_lft;
	__u32 prefered_lft;
	int addr_type;
	struct inet6_dev *in6_dev;
	struct net *net = dev_net(dev);

	pinfo = (struct prefix_info *) opt;

	if (len < sizeof(struct prefix_info)) {
		ADBG(("addrconf: prefix option too short\n"));
		return;
	}

	/*
	 *	Validation checks ([ADDRCONF], page 19)
	 */

	addr_type = ipv6_addr_type(&pinfo->prefix);

	if (addr_type & (IPV6_ADDR_MULTICAST|IPV6_ADDR_LINKLOCAL))
		return;

	valid_lft = ntohl(pinfo->valid);
	prefered_lft = ntohl(pinfo->prefered);

	if (prefered_lft > valid_lft) {
		net_warn_ratelimited("addrconf: prefix option has invalid lifetime\n");
		return;
	}

	in6_dev = in6_dev_get(dev);

	if (in6_dev == NULL) {
		net_dbg_ratelimited("addrconf: device %s not configured\n",
				    dev->name);
		return;
	}

	/*
	 *	Two things going on here:
	 *	1) Add routes for on-link prefixes
	 *	2) Configure prefixes with the auto flag set
	 */

	if (pinfo->onlink) {
		struct rt6_info *rt;
		unsigned long rt_expires;

		/* Avoid arithmetic overflow. Really, we could
		 * save rt_expires in seconds, likely valid_lft,
		 * but it would require division in fib gc, that it
		 * not good.
		 */
		if (HZ > USER_HZ)
			rt_expires = addrconf_timeout_fixup(valid_lft, HZ);
		else
			rt_expires = addrconf_timeout_fixup(valid_lft, USER_HZ);

		if (addrconf_finite_timeout(rt_expires))
			rt_expires *= HZ;

		rt = addrconf_get_prefix_route(&pinfo->prefix,
					       pinfo->prefix_len,
					       dev,
					       RTF_ADDRCONF | RTF_PREFIX_RT,
					       RTF_GATEWAY | RTF_DEFAULT);

		if (rt) {
			/* Autoconf prefix route */
			if (valid_lft == 0) {
				ip6_del_rt(rt);
				rt = NULL;
			} else if (addrconf_finite_timeout(rt_expires)) {
				/* not infinity */
				rt6_set_expires(rt, jiffies + rt_expires);
			} else {
				rt6_clean_expires(rt);
			}
		} else if (valid_lft) {
			clock_t expires = 0;
			int flags = RTF_ADDRCONF | RTF_PREFIX_RT;
			if (addrconf_finite_timeout(rt_expires)) {
				/* not infinity */
				flags |= RTF_EXPIRES;
				expires = jiffies_to_clock_t(rt_expires);
			}
			addrconf_prefix_route(&pinfo->prefix, pinfo->prefix_len,
					      dev, expires, flags);
		}
		ip6_rt_put(rt);
	}

	/* Try to figure out our local address for this prefix */

	if (pinfo->autoconf && in6_dev->cnf.autoconf) {
		struct inet6_ifaddr *ifp;
		struct in6_addr addr;
		int create = 0, update_lft = 0;
		bool tokenized = false;

		if (pinfo->prefix_len == 64) {
			memcpy(&addr, &pinfo->prefix, 8);

			if (!ipv6_addr_any(&in6_dev->token)) {
				read_lock_bh(&in6_dev->lock);
				memcpy(addr.s6_addr + 8,
				       in6_dev->token.s6_addr + 8, 8);
				read_unlock_bh(&in6_dev->lock);
				tokenized = true;
			} else if (ipv6_generate_eui64(addr.s6_addr + 8, dev) &&
				   ipv6_inherit_eui64(addr.s6_addr + 8, in6_dev)) {
				in6_dev_put(in6_dev);
				return;
			}
			goto ok;
		}
		net_dbg_ratelimited("IPv6 addrconf: prefix with wrong length %d\n",
				    pinfo->prefix_len);
		in6_dev_put(in6_dev);
		return;

ok:

		ifp = ipv6_get_ifaddr(net, &addr, dev, 1);

		if (ifp == NULL && valid_lft) {
			int max_addresses = in6_dev->cnf.max_addresses;
			u32 addr_flags = 0;

#ifdef CONFIG_IPV6_OPTIMISTIC_DAD
			if (in6_dev->cnf.optimistic_dad &&
			    !net->ipv6.devconf_all->forwarding && sllao)
				addr_flags = IFA_F_OPTIMISTIC;
#endif

			/* Do not allow to create too much of autoconfigured
			 * addresses; this would be too easy way to crash kernel.
			 */
			if (!max_addresses ||
			    ipv6_count_addresses(in6_dev) < max_addresses)
				ifp = ipv6_add_addr(in6_dev, &addr, NULL,
						    pinfo->prefix_len,
						    addr_type&IPV6_ADDR_SCOPE_MASK,
						    addr_flags, valid_lft,
						    prefered_lft);

			if (IS_ERR_OR_NULL(ifp)) {
				in6_dev_put(in6_dev);
				return;
			}

			update_lft = 0;
			create = 1;
			ifp->cstamp = jiffies;
			ifp->tokenized = tokenized;
			addrconf_dad_start(ifp);
		}

		if (ifp) {
			int flags;
			unsigned long now;
#ifdef CONFIG_IPV6_PRIVACY
			struct inet6_ifaddr *ift;
#endif
			u32 stored_lft;

			/* update lifetime (RFC2462 5.5.3 e) */
			spin_lock(&ifp->lock);
			now = jiffies;
			if (ifp->valid_lft > (now - ifp->tstamp) / HZ)
				stored_lft = ifp->valid_lft - (now - ifp->tstamp) / HZ;
			else
				stored_lft = 0;
			if (!update_lft && !create && stored_lft) {
				if (valid_lft > MIN_VALID_LIFETIME ||
				    valid_lft > stored_lft)
					update_lft = 1;
				else if (stored_lft <= MIN_VALID_LIFETIME) {
					/* valid_lft <= stored_lft is always true */
					/*
					 * RFC 4862 Section 5.5.3e:
					 * "Note that the preferred lifetime of
					 *  the corresponding address is always
					 *  reset to the Preferred Lifetime in
					 *  the received Prefix Information
					 *  option, regardless of whether the
					 *  valid lifetime is also reset or
					 *  ignored."
					 *
					 *  So if the preferred lifetime in
					 *  this advertisement is different
					 *  than what we have stored, but the
					 *  valid lifetime is invalid, just
					 *  reset prefered_lft.
					 *
					 *  We must set the valid lifetime
					 *  to the stored lifetime since we'll
					 *  be updating the timestamp below,
					 *  else we'll set it back to the
					 *  minimum.
					 */
					if (prefered_lft != ifp->prefered_lft) {
						valid_lft = stored_lft;
						update_lft = 1;
					}
				} else {
					valid_lft = MIN_VALID_LIFETIME;
					if (valid_lft < prefered_lft)
						prefered_lft = valid_lft;
					update_lft = 1;
				}
			}

			if (update_lft) {
				ifp->valid_lft = valid_lft;
				ifp->prefered_lft = prefered_lft;
				ifp->tstamp = now;
				flags = ifp->flags;
				ifp->flags &= ~IFA_F_DEPRECATED;
				spin_unlock(&ifp->lock);

				if (!(flags&IFA_F_TENTATIVE))
					ipv6_ifa_notify(0, ifp);
			} else
				spin_unlock(&ifp->lock);

#ifdef CONFIG_IPV6_PRIVACY
			read_lock_bh(&in6_dev->lock);
			/* update all temporary addresses in the list */
			list_for_each_entry(ift, &in6_dev->tempaddr_list,
					    tmp_list) {
				int age, max_valid, max_prefered;

				if (ifp != ift->ifpub)
					continue;

				/*
				 * RFC 4941 section 3.3:
				 * If a received option will extend the lifetime
				 * of a public address, the lifetimes of
				 * temporary addresses should be extended,
				 * subject to the overall constraint that no
				 * temporary addresses should ever remain
				 * "valid" or "preferred" for a time longer than
				 * (TEMP_VALID_LIFETIME) or
				 * (TEMP_PREFERRED_LIFETIME - DESYNC_FACTOR),
				 * respectively.
				 */
				age = (now - ift->cstamp) / HZ;
				max_valid = in6_dev->cnf.temp_valid_lft - age;
				if (max_valid < 0)
					max_valid = 0;

				max_prefered = in6_dev->cnf.temp_prefered_lft -
					       in6_dev->cnf.max_desync_factor -
					       age;
				if (max_prefered < 0)
					max_prefered = 0;

				if (valid_lft > max_valid)
					valid_lft = max_valid;

				if (prefered_lft > max_prefered)
					prefered_lft = max_prefered;

				spin_lock(&ift->lock);
				flags = ift->flags;
				ift->valid_lft = valid_lft;
				ift->prefered_lft = prefered_lft;
				ift->tstamp = now;
				if (prefered_lft > 0)
					ift->flags &= ~IFA_F_DEPRECATED;

				spin_unlock(&ift->lock);
				if (!(flags&IFA_F_TENTATIVE))
					ipv6_ifa_notify(0, ift);
			}

			if ((create || list_empty(&in6_dev->tempaddr_list)) && in6_dev->cnf.use_tempaddr > 0) {
				/*
				 * When a new public address is created as
				 * described in [ADDRCONF], also create a new
				 * temporary address. Also create a temporary
				 * address if it's enabled but no temporary
				 * address currently exists.
				 */
				read_unlock_bh(&in6_dev->lock);
				ipv6_create_tempaddr(ifp, NULL);
			} else {
				read_unlock_bh(&in6_dev->lock);
			}
#endif
			in6_ifa_put(ifp);
			addrconf_verify(0);
		}
	}
	inet6_prefix_notify(RTM_NEWPREFIX, in6_dev, pinfo);
	in6_dev_put(in6_dev);
}