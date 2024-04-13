static void __exit bonding_exit(void)
{
	unregister_netdevice_notifier(&bond_netdev_notifier);

	bond_destroy_debugfs();

	bond_netlink_fini();
	unregister_pernet_subsys(&bond_net_ops);

#ifdef CONFIG_NET_POLL_CONTROLLER
	/* Make sure we don't have an imbalance on our netpoll blocking */
	WARN_ON(atomic_read(&netpoll_block_tx));
#endif
}