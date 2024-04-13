void bpf_map_inc(struct bpf_map *map)
{
	atomic64_inc(&map->refcnt);
}