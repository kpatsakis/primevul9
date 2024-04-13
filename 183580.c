static void __net_exit igmp_net_exit(struct net *net)
{
	remove_proc_entry("mcfilter", net->proc_net);
	remove_proc_entry("igmp", net->proc_net);
	inet_ctl_sock_destroy(net->ipv4.mc_autojoin_sk);
}