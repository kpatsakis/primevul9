static void *fd_htab_map_get_ptr(const struct bpf_map *map, struct htab_elem *l)
{
	return *(void **)(l->key + roundup(map->key_size, 8));
}