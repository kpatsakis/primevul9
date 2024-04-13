static void *htab_of_map_lookup_elem(struct bpf_map *map, void *key)
{
	struct bpf_map **inner_map  = htab_map_lookup_elem(map, key);

	if (!inner_map)
		return NULL;

	return READ_ONCE(*inner_map);
}