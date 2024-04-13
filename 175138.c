static int __net_init ovs_init_net(struct net *net)
{
	struct ovs_net *ovs_net = net_generic(net, ovs_net_id);

	INIT_LIST_HEAD(&ovs_net->dps);
	INIT_WORK(&ovs_net->dp_notify_work, ovs_dp_notify_wq);
	return 0;
}