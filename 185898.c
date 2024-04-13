static void __net_exit ip6_route_net_exit_late(struct net *net)
{
#ifdef CONFIG_PROC_FS
	remove_proc_entry("ipv6_route", net->proc_net);
	remove_proc_entry("rt6_stats", net->proc_net);
#endif
}