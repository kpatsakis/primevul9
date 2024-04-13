static int htab_lru_percpu_map_lookup_and_delete_elem(struct bpf_map *map,
						      void *key, void *value,
						      u64 flags)
{
	return __htab_map_lookup_and_delete_elem(map, key, value, true, true,
						 flags);
}