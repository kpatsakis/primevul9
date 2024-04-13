static void htab_of_map_free(struct bpf_map *map)
{
	bpf_map_meta_free(map->inner_map_meta);
	fd_htab_map_free(map);
}