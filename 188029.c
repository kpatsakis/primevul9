static void __net_exit packet_net_exit(struct net *net)
{
	proc_net_remove(net, "packet");
}