static void *bpf_hash_map_seq_start(struct seq_file *seq, loff_t *pos)
{
	struct bpf_iter_seq_hash_map_info *info = seq->private;
	struct htab_elem *elem;

	elem = bpf_hash_map_seq_find_next(info, NULL);
	if (!elem)
		return NULL;

	if (*pos == 0)
		++*pos;
	return elem;
}