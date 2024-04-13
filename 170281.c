static void maybe_wait_bpf_programs(struct bpf_map *map)
{
	/* Wait for any running BPF programs to complete so that
	 * userspace, when we return to it, knows that all programs
	 * that could be running use the new map value.
	 */
	if (map->map_type == BPF_MAP_TYPE_HASH_OF_MAPS ||
	    map->map_type == BPF_MAP_TYPE_ARRAY_OF_MAPS)
		synchronize_rcu();
}