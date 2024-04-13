static void *netlink_seq_start(struct seq_file *seq, loff_t *posp)
{
	struct nl_seq_iter *iter = seq->private;
	void *obj = SEQ_START_TOKEN;
	loff_t pos;
	int err;

	iter->link = 0;

	err = netlink_walk_start(iter);
	if (err)
		return ERR_PTR(err);

	for (pos = *posp; pos && obj && !IS_ERR(obj); pos--)
		obj = __netlink_seq_next(seq);

	return obj;
}