static int __net_init cangw_pernet_init(struct net *net)
{
	INIT_HLIST_HEAD(&net->can.cgw_list);
	return 0;
}