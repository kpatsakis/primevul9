static __net_init int proto_init_net(struct net *net)
{
	if (!proc_create("protocols", S_IRUGO, net->proc_net, &proto_seq_fops))
		return -ENOMEM;

	return 0;
}