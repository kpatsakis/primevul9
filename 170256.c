void bpf_map_put(struct bpf_map *map)
{
	__bpf_map_put(map, true);
}