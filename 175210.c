static struct nfnl_queue_net *nfnl_queue_pernet(struct net *net)
{
	return net_generic(net, nfnl_queue_net_id);
}