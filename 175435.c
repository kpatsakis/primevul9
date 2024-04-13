static int __bpf_hash_map_seq_show(struct seq_file *seq, struct htab_elem *elem)
{
	struct bpf_iter_seq_hash_map_info *info = seq->private;
	u32 roundup_key_size, roundup_value_size;
	struct bpf_iter__bpf_map_elem ctx = {};
	struct bpf_map *map = info->map;
	struct bpf_iter_meta meta;
	int ret = 0, off = 0, cpu;
	struct bpf_prog *prog;
	void __percpu *pptr;

	meta.seq = seq;
	prog = bpf_iter_get_info(&meta, elem == NULL);
	if (prog) {
		ctx.meta = &meta;
		ctx.map = info->map;
		if (elem) {
			roundup_key_size = round_up(map->key_size, 8);
			ctx.key = elem->key;
			if (!info->percpu_value_buf) {
				ctx.value = elem->key + roundup_key_size;
			} else {
				roundup_value_size = round_up(map->value_size, 8);
				pptr = htab_elem_get_ptr(elem, map->key_size);
				for_each_possible_cpu(cpu) {
					bpf_long_memcpy(info->percpu_value_buf + off,
							per_cpu_ptr(pptr, cpu),
							roundup_value_size);
					off += roundup_value_size;
				}
				ctx.value = info->percpu_value_buf;
			}
		}
		ret = bpf_iter_run_prog(prog, &ctx);
	}

	return ret;
}