static __exit void cgw_module_exit(void)
{
	rtnl_unregister_all(PF_CAN);

	unregister_netdevice_notifier(&notifier);

	unregister_pernet_subsys(&cangw_pernet_ops);
	rcu_barrier(); /* Wait for completion of call_rcu()'s */

	kmem_cache_destroy(cgw_cache);
}