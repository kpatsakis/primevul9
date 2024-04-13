struct hlist_node *seq_hlist_next_rcu(void *v,
				      struct hlist_head *head,
				      loff_t *ppos)
{
	struct hlist_node *node = v;

	++*ppos;
	if (v == SEQ_START_TOKEN)
		return rcu_dereference(head->first);
	else
		return rcu_dereference(node->next);
}