__instance_destroy(struct nfqnl_instance *inst)
{
	hlist_del_rcu(&inst->hlist);
	call_rcu(&inst->rcu, instance_destroy_rcu);
}