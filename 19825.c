struct hlist_node *seq_hlist_start_rcu(struct hlist_head *head,
				       loff_t pos)
{
	struct hlist_node *node;

	__hlist_for_each_rcu(node, head)
		if (pos-- == 0)
			return node;
	return NULL;
}