static void bpf_hash_map_seq_stop(struct seq_file *seq, void *v)
{
	if (!v)
		(void)__bpf_hash_map_seq_show(seq, NULL);
	else
		rcu_read_unlock();
}