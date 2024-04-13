static int bpf_iter_init_hash_map(void *priv_data,
				  struct bpf_iter_aux_info *aux)
{
	struct bpf_iter_seq_hash_map_info *seq_info = priv_data;
	struct bpf_map *map = aux->map;
	void *value_buf;
	u32 buf_size;

	if (map->map_type == BPF_MAP_TYPE_PERCPU_HASH ||
	    map->map_type == BPF_MAP_TYPE_LRU_PERCPU_HASH) {
		buf_size = round_up(map->value_size, 8) * num_possible_cpus();
		value_buf = kmalloc(buf_size, GFP_USER | __GFP_NOWARN);
		if (!value_buf)
			return -ENOMEM;

		seq_info->percpu_value_buf = value_buf;
	}

	seq_info->map = map;
	seq_info->htab = container_of(map, struct bpf_htab, map);
	return 0;
}