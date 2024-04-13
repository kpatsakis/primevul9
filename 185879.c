static int __net_init ip6_route_net_init_late(struct net *net)
{
#ifdef CONFIG_PROC_FS
	proc_create("ipv6_route", 0, net->proc_net, &ipv6_route_proc_fops);
	proc_create("rt6_stats", S_IRUGO, net->proc_net, &rt6_stats_seq_fops);
#endif
	return 0;
}