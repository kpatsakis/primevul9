instance_lookup(struct nfnl_queue_net *q, u_int16_t queue_num)
{
	struct hlist_head *head;
	struct nfqnl_instance *inst;

	head = &q->instance_table[instance_hashfn(queue_num)];
	hlist_for_each_entry_rcu(inst, head, hlist) {
		if (inst->queue_num == queue_num)
			return inst;
	}
	return NULL;
}