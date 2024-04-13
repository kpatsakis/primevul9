static struct hlist_node *get_next(struct seq_file *seq, struct hlist_node *h)
{
	struct iter_state *st = seq->private;
	struct net *net = seq_file_net(seq);

	h = h->next;
	while (!h) {
		struct nfnl_queue_net *q;

		if (++st->bucket >= INSTANCE_BUCKETS)
			return NULL;

		q = nfnl_queue_pernet(net);
		h = q->instance_table[st->bucket].first;
	}
	return h;
}