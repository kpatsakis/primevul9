static const struct bpf_map *bpf_map_from_imm(const struct bpf_prog *prog,
					      unsigned long addr, u32 *off,
					      u32 *type)
{
	const struct bpf_map *map;
	int i;

	mutex_lock(&prog->aux->used_maps_mutex);
	for (i = 0, *off = 0; i < prog->aux->used_map_cnt; i++) {
		map = prog->aux->used_maps[i];
		if (map == (void *)addr) {
			*type = BPF_PSEUDO_MAP_FD;
			goto out;
		}
		if (!map->ops->map_direct_value_meta)
			continue;
		if (!map->ops->map_direct_value_meta(map, addr, off)) {
			*type = BPF_PSEUDO_MAP_VALUE;
			goto out;
		}
	}
	map = NULL;

out:
	mutex_unlock(&prog->aux->used_maps_mutex);
	return map;
}