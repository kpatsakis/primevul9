void addrconf_cleanup(void)
{
	struct net_device *dev;
	int i;

	unregister_netdevice_notifier(&ipv6_dev_notf);
	unregister_pernet_subsys(&addrconf_ops);
	ipv6_addr_label_cleanup();

	rtnl_lock();

	__rtnl_af_unregister(&inet6_ops);

	/* clean dev list */
	for_each_netdev(&init_net, dev) {
		if (__in6_dev_get(dev) == NULL)
			continue;
		addrconf_ifdown(dev, 1);
	}
	addrconf_ifdown(init_net.loopback_dev, 2);

	/*
	 *	Check hash table.
	 */
	spin_lock_bh(&addrconf_hash_lock);
	for (i = 0; i < IN6_ADDR_HSIZE; i++)
		WARN_ON(!hlist_empty(&inet6_addr_lst[i]));
	spin_unlock_bh(&addrconf_hash_lock);

	del_timer(&addr_chk_timer);
	rtnl_unlock();
}