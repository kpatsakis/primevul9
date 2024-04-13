static void __net_exit nfnl_queue_net_exit(struct net *net)
{
#ifdef CONFIG_PROC_FS
	remove_proc_entry("nfnetlink_queue", net->nf.proc_netfilter);
#endif
}