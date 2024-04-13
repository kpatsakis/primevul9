static void *htab_lru_map_lookup_elem(struct bpf_map *map, void *key)
{
	return __htab_lru_map_lookup_elem(map, key, true);
}