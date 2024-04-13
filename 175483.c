static void bpf_iter_fini_hash_map(void *priv_data)
{
	struct bpf_iter_seq_hash_map_info *seq_info = priv_data;

	kfree(seq_info->percpu_value_buf);
}