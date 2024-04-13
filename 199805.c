static void __net_exit if6_proc_net_exit(struct net *net)
{
	remove_proc_entry("if_inet6", net->proc_net);
}