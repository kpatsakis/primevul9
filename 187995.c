static int __net_init packet_net_init(struct net *net)
{
	spin_lock_init(&net->packet.sklist_lock);
	INIT_HLIST_HEAD(&net->packet.sklist);

	if (!proc_net_fops_create(net, "packet", 0, &packet_seq_fops))
		return -ENOMEM;

	return 0;
}