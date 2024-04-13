static struct bpf_map *htab_of_map_alloc(union bpf_attr *attr)
{
	struct bpf_map *map, *inner_map_meta;

	inner_map_meta = bpf_map_meta_alloc(attr->inner_map_fd);
	if (IS_ERR(inner_map_meta))
		return inner_map_meta;

	map = htab_map_alloc(attr);
	if (IS_ERR(map)) {
		bpf_map_meta_free(inner_map_meta);
		return map;
	}

	map->inner_map_meta = inner_map_meta;

	return map;
}