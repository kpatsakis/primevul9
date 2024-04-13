void bpf_map_inc_with_uref(struct bpf_map *map)
{
	atomic64_inc(&map->refcnt);
	atomic64_inc(&map->usercnt);
}