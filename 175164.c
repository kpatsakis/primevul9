instance_destroy_rcu(struct rcu_head *head)
{
	struct nfqnl_instance *inst = container_of(head, struct nfqnl_instance,
						   rcu);

	nfqnl_flush(inst, NULL, 0);
	kfree(inst);
	module_put(THIS_MODULE);
}