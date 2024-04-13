static int htab_lru_percpu_map_update_elem(struct bpf_map *map, void *key,
					   void *value, u64 map_flags)
{
	return __htab_lru_percpu_map_update_elem(map, key, value, map_flags,
						 false);
}