nfqnl_dev_drop(struct net *net, int ifindex)
{
	int i;
	struct nfnl_queue_net *q = nfnl_queue_pernet(net);

	rcu_read_lock();

	for (i = 0; i < INSTANCE_BUCKETS; i++) {
		struct nfqnl_instance *inst;
		struct hlist_head *head = &q->instance_table[i];

		hlist_for_each_entry_rcu(inst, head, hlist)
			nfqnl_flush(inst, dev_cmp, ifindex);
	}

	rcu_read_unlock();
}