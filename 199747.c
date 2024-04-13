static void ipv6_del_addr(struct inet6_ifaddr *ifp)
{
	struct inet6_ifaddr *ifa, *ifn;
	struct inet6_dev *idev = ifp->idev;
	int state;
	int deleted = 0, onlink = 0;
	unsigned long expires = jiffies;

	spin_lock_bh(&ifp->state_lock);
	state = ifp->state;
	ifp->state = INET6_IFADDR_STATE_DEAD;
	spin_unlock_bh(&ifp->state_lock);

	if (state == INET6_IFADDR_STATE_DEAD)
		goto out;

	spin_lock_bh(&addrconf_hash_lock);
	hlist_del_init_rcu(&ifp->addr_lst);
	spin_unlock_bh(&addrconf_hash_lock);

	write_lock_bh(&idev->lock);
#ifdef CONFIG_IPV6_PRIVACY
	if (ifp->flags&IFA_F_TEMPORARY) {
		list_del(&ifp->tmp_list);
		if (ifp->ifpub) {
			in6_ifa_put(ifp->ifpub);
			ifp->ifpub = NULL;
		}
		__in6_ifa_put(ifp);
	}
#endif

	list_for_each_entry_safe(ifa, ifn, &idev->addr_list, if_list) {
		if (ifa == ifp) {
			list_del_init(&ifp->if_list);
			__in6_ifa_put(ifp);

			if (!(ifp->flags & IFA_F_PERMANENT) || onlink > 0)
				break;
			deleted = 1;
			continue;
		} else if (ifp->flags & IFA_F_PERMANENT) {
			if (ipv6_prefix_equal(&ifa->addr, &ifp->addr,
					      ifp->prefix_len)) {
				if (ifa->flags & IFA_F_PERMANENT) {
					onlink = 1;
					if (deleted)
						break;
				} else {
					unsigned long lifetime;

					if (!onlink)
						onlink = -1;

					spin_lock(&ifa->lock);

					lifetime = addrconf_timeout_fixup(ifa->valid_lft, HZ);
					/*
					 * Note: Because this address is
					 * not permanent, lifetime <
					 * LONG_MAX / HZ here.
					 */
					if (time_before(expires,
							ifa->tstamp + lifetime * HZ))
						expires = ifa->tstamp + lifetime * HZ;
					spin_unlock(&ifa->lock);
				}
			}
		}
	}
	write_unlock_bh(&idev->lock);

	addrconf_del_dad_timer(ifp);

	ipv6_ifa_notify(RTM_DELADDR, ifp);

	inet6addr_notifier_call_chain(NETDEV_DOWN, ifp);

	/*
	 * Purge or update corresponding prefix
	 *
	 * 1) we don't purge prefix here if address was not permanent.
	 *    prefix is managed by its own lifetime.
	 * 2) if there're no addresses, delete prefix.
	 * 3) if there're still other permanent address(es),
	 *    corresponding prefix is still permanent.
	 * 4) otherwise, update prefix lifetime to the
	 *    longest valid lifetime among the corresponding
	 *    addresses on the device.
	 *    Note: subsequent RA will update lifetime.
	 *
	 * --yoshfuji
	 */
	if ((ifp->flags & IFA_F_PERMANENT) && onlink < 1) {
		struct in6_addr prefix;
		struct rt6_info *rt;

		ipv6_addr_prefix(&prefix, &ifp->addr, ifp->prefix_len);

		rt = addrconf_get_prefix_route(&prefix,
					       ifp->prefix_len,
					       ifp->idev->dev,
					       0, RTF_GATEWAY | RTF_DEFAULT);

		if (rt) {
			if (onlink == 0) {
				ip6_del_rt(rt);
				rt = NULL;
			} else if (!(rt->rt6i_flags & RTF_EXPIRES)) {
				rt6_set_expires(rt, expires);
			}
		}
		ip6_rt_put(rt);
	}

	/* clean up prefsrc entries */
	rt6_remove_prefsrc(ifp);
out:
	in6_ifa_put(ifp);
}