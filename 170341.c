static void bpf_map_show_fdinfo(struct seq_file *m, struct file *filp)
{
	const struct bpf_map *map = filp->private_data;
	const struct bpf_array *array;
	u32 type = 0, jited = 0;

	if (map->map_type == BPF_MAP_TYPE_PROG_ARRAY) {
		array = container_of(map, struct bpf_array, map);
		type  = array->aux->type;
		jited = array->aux->jited;
	}

	seq_printf(m,
		   "map_type:\t%u\n"
		   "key_size:\t%u\n"
		   "value_size:\t%u\n"
		   "max_entries:\t%u\n"
		   "map_flags:\t%#x\n"
		   "memlock:\t%lu\n"
		   "map_id:\t%u\n"
		   "frozen:\t%u\n",
		   map->map_type,
		   map->key_size,
		   map->value_size,
		   map->max_entries,
		   map->map_flags,
		   bpf_map_memory_footprint(map),
		   map->id,
		   READ_ONCE(map->frozen));
	if (type) {
		seq_printf(m, "owner_prog_type:\t%u\n", type);
		seq_printf(m, "owner_jited:\t%u\n", jited);
	}
}