verdict_instance_lookup(struct nfnl_queue_net *q, u16 queue_num, int nlportid)
{
	struct nfqnl_instance *queue;

	queue = instance_lookup(q, queue_num);
	if (!queue)
		return ERR_PTR(-ENODEV);

	if (queue->peer_portid != nlportid)
		return ERR_PTR(-EPERM);

	return queue;
}