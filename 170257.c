static unsigned long bpf_map_memory_footprint(const struct bpf_map *map)
{
	unsigned long size;

	size = round_up(map->key_size + bpf_map_value_size(map), 8);

	return round_up(map->max_entries * size, PAGE_SIZE);
}