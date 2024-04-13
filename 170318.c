static struct bpf_map *__bpf_map_inc_not_zero(struct bpf_map *map, bool uref)
{
	int refold;

	refold = atomic64_fetch_add_unless(&map->refcnt, 1, 0);
	if (!refold)
		return ERR_PTR(-ENOENT);
	if (uref)
		atomic64_inc(&map->usercnt);

	return map;
}