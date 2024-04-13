static void __exit nf_tables_module_exit(void)
{
	unregister_pernet_subsys(&nf_tables_net_ops);
	nfnetlink_subsys_unregister(&nf_tables_subsys);
	rcu_barrier();
	nf_tables_core_module_exit();
	kfree(info);
}