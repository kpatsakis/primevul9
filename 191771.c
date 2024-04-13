static void __net_exit netlink_net_exit(struct net *net)
{
#ifdef CONFIG_PROC_FS
	remove_proc_entry("netlink", net->proc_net);
#endif
}