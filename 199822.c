void if6_proc_exit(void)
{
	unregister_pernet_subsys(&if6_proc_net_ops);
}