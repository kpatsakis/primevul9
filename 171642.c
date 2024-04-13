static void __net_exit sock_inuse_exit_net(struct net *net)
{
	free_percpu(net->core.inuse);
}