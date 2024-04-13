static void htab_put_fd_value(struct bpf_htab *htab, struct htab_elem *l)
{
	struct bpf_map *map = &htab->map;
	void *ptr;

	if (map->ops->map_fd_put_ptr) {
		ptr = fd_htab_map_get_ptr(map, l);
		map->ops->map_fd_put_ptr(ptr);
	}
}