static void netlink_seq_stop(struct seq_file *seq, void *v)
{
	struct nl_seq_iter *iter = seq->private;

	if (iter->link >= MAX_LINKS)
		return;

	netlink_walk_stop(iter);
}