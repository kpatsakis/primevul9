static struct hlist_node *get_idx(struct seq_file *seq, loff_t pos)
{
	struct hlist_node *head;
	head = get_first(seq);

	if (head)
		while (pos && (head = get_next(seq, head)))
			pos--;
	return pos ? NULL : head;
}