int __init addrconf_init(void)
{
	int i, err;

	err = ipv6_addr_label_init();
	if (err < 0) {
		pr_crit("%s: cannot initialize default policy table: %d\n",
			__func__, err);
		goto out;
	}

	err = register_pernet_subsys(&addrconf_ops);
	if (err < 0)
		goto out_addrlabel;

	/* The addrconf netdev notifier requires that loopback_dev
	 * has it's ipv6 private information allocated and setup
	 * before it can bring up and give link-local addresses
	 * to other devices which are up.
	 *
	 * Unfortunately, loopback_dev is not necessarily the first
	 * entry in the global dev_base list of net devices.  In fact,
	 * it is likely to be the very last entry on that list.
	 * So this causes the notifier registry below to try and
	 * give link-local addresses to all devices besides loopback_dev
	 * first, then loopback_dev, which cases all the non-loopback_dev
	 * devices to fail to get a link-local address.
	 *
	 * So, as a temporary fix, allocate the ipv6 structure for
	 * loopback_dev first by hand.
	 * Longer term, all of the dependencies ipv6 has upon the loopback
	 * device and it being up should be removed.
	 */
	rtnl_lock();
	if (!ipv6_add_dev(init_net.loopback_dev))
		err = -ENOMEM;
	rtnl_unlock();
	if (err)
		goto errlo;

	for (i = 0; i < IN6_ADDR_HSIZE; i++)
		INIT_HLIST_HEAD(&inet6_addr_lst[i]);

	register_netdevice_notifier(&ipv6_dev_notf);

	addrconf_verify(0);

	err = rtnl_af_register(&inet6_ops);
	if (err < 0)
		goto errout_af;

	err = __rtnl_register(PF_INET6, RTM_GETLINK, NULL, inet6_dump_ifinfo,
			      NULL);
	if (err < 0)
		goto errout;

	/* Only the first call to __rtnl_register can fail */
	__rtnl_register(PF_INET6, RTM_NEWADDR, inet6_rtm_newaddr, NULL, NULL);
	__rtnl_register(PF_INET6, RTM_DELADDR, inet6_rtm_deladdr, NULL, NULL);
	__rtnl_register(PF_INET6, RTM_GETADDR, inet6_rtm_getaddr,
			inet6_dump_ifaddr, NULL);
	__rtnl_register(PF_INET6, RTM_GETMULTICAST, NULL,
			inet6_dump_ifmcaddr, NULL);
	__rtnl_register(PF_INET6, RTM_GETANYCAST, NULL,
			inet6_dump_ifacaddr, NULL);
	__rtnl_register(PF_INET6, RTM_GETNETCONF, inet6_netconf_get_devconf,
			inet6_netconf_dump_devconf, NULL);

	ipv6_addr_label_rtnl_register();

	return 0;
errout:
	rtnl_af_unregister(&inet6_ops);
errout_af:
	unregister_netdevice_notifier(&ipv6_dev_notf);
errlo:
	unregister_pernet_subsys(&addrconf_ops);
out_addrlabel:
	ipv6_addr_label_cleanup();
out:
	return err;
}