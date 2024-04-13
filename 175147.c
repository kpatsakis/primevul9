static void __exit nfnetlink_queue_fini(void)
{
	nf_unregister_queue_handler();
	unregister_netdevice_notifier(&nfqnl_dev_notifier);
	unregister_pernet_subsys(&nfnl_queue_net_ops);
	nfnetlink_subsys_unregister(&nfqnl_subsys);
	netlink_unregister_notifier(&nfqnl_rtnl_notifier);

	rcu_barrier(); /* Wait for completion of call_rcu()'s */
}