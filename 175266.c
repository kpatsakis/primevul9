instance_destroy(struct nfnl_queue_net *q, struct nfqnl_instance *inst)
{
	spin_lock(&q->instances_lock);
	__instance_destroy(inst);
	spin_unlock(&q->instances_lock);
}