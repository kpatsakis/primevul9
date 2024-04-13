static void *bpf_hash_map_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	struct bpf_iter_seq_hash_map_info *info = seq->private;

	++*pos;
	++info->skip_elems;
	return bpf_hash_map_seq_find_next(info, v);
}