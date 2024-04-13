static struct hlist_node *get_first(struct seq_file *seq)
{
	struct iter_state *st = seq->private;
	struct net *net;
	struct nfnl_queue_net *q;

	if (!st)
		return NULL;

	net = seq_file_net(seq);
	q = nfnl_queue_pernet(net);
	for (st->bucket = 0; st->bucket < INSTANCE_BUCKETS; st->bucket++) {
		if (!hlist_empty(&q->instance_table[st->bucket]))
			return q->instance_table[st->bucket].first;
	}
	return NULL;
}