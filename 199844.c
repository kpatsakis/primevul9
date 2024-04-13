static int __net_init if6_proc_net_init(struct net *net)
{
	if (!proc_create("if_inet6", S_IRUGO, net->proc_net, &if6_fops))
		return -ENOMEM;
	return 0;
}