static struct inet6_dev *ipv6_add_dev(struct net_device *dev)
{
	struct inet6_dev *ndev;

	ASSERT_RTNL();

	if (dev->mtu < IPV6_MIN_MTU)
		return NULL;

	ndev = kzalloc(sizeof(struct inet6_dev), GFP_KERNEL);

	if (ndev == NULL)
		return NULL;

	rwlock_init(&ndev->lock);
	ndev->dev = dev;
	INIT_LIST_HEAD(&ndev->addr_list);
	setup_timer(&ndev->rs_timer, addrconf_rs_timer,
		    (unsigned long)ndev);
	memcpy(&ndev->cnf, dev_net(dev)->ipv6.devconf_dflt, sizeof(ndev->cnf));
	ndev->cnf.mtu6 = dev->mtu;
	ndev->cnf.sysctl = NULL;
	ndev->nd_parms = neigh_parms_alloc(dev, &nd_tbl);
	if (ndev->nd_parms == NULL) {
		kfree(ndev);
		return NULL;
	}
	if (ndev->cnf.forwarding)
		dev_disable_lro(dev);
	/* We refer to the device */
	dev_hold(dev);

	if (snmp6_alloc_dev(ndev) < 0) {
		ADBG((KERN_WARNING
			"%s: cannot allocate memory for statistics; dev=%s.\n",
			__func__, dev->name));
		neigh_parms_release(&nd_tbl, ndev->nd_parms);
		dev_put(dev);
		kfree(ndev);
		return NULL;
	}

	if (snmp6_register_dev(ndev) < 0) {
		ADBG((KERN_WARNING
			"%s: cannot create /proc/net/dev_snmp6/%s\n",
			__func__, dev->name));
		neigh_parms_release(&nd_tbl, ndev->nd_parms);
		ndev->dead = 1;
		in6_dev_finish_destroy(ndev);
		return NULL;
	}

	/* One reference from device.  We must do this before
	 * we invoke __ipv6_regen_rndid().
	 */
	in6_dev_hold(ndev);

	if (dev->flags & (IFF_NOARP | IFF_LOOPBACK))
		ndev->cnf.accept_dad = -1;

#if IS_ENABLED(CONFIG_IPV6_SIT)
	if (dev->type == ARPHRD_SIT && (dev->priv_flags & IFF_ISATAP)) {
		pr_info("%s: Disabled Multicast RS\n", dev->name);
		ndev->cnf.rtr_solicits = 0;
	}
#endif

#ifdef CONFIG_IPV6_PRIVACY
	INIT_LIST_HEAD(&ndev->tempaddr_list);
	setup_timer(&ndev->regen_timer, ipv6_regen_rndid, (unsigned long)ndev);
	if ((dev->flags&IFF_LOOPBACK) ||
	    dev->type == ARPHRD_TUNNEL ||
	    dev->type == ARPHRD_TUNNEL6 ||
	    dev->type == ARPHRD_SIT ||
	    dev->type == ARPHRD_NONE) {
		ndev->cnf.use_tempaddr = -1;
	} else {
		in6_dev_hold(ndev);
		ipv6_regen_rndid((unsigned long) ndev);
	}
#endif
	ndev->token = in6addr_any;

	if (netif_running(dev) && addrconf_qdisc_ok(dev))
		ndev->if_flags |= IF_READY;

	ipv6_mc_init_dev(ndev);
	ndev->tstamp = jiffies;
	addrconf_sysctl_register(ndev);
	/* protected by rtnl_lock */
	rcu_assign_pointer(dev->ip6_ptr, ndev);

	/* Join interface-local all-node multicast group */
	ipv6_dev_mc_inc(dev, &in6addr_interfacelocal_allnodes);

	/* Join all-node multicast group */
	ipv6_dev_mc_inc(dev, &in6addr_linklocal_allnodes);

	/* Join all-router multicast group if forwarding is set */
	if (ndev->cnf.forwarding && (dev->flags & IFF_MULTICAST))
		ipv6_dev_mc_inc(dev, &in6addr_linklocal_allrouters);

	return ndev;
}