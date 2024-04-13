void *bpf_map_kzalloc(const struct bpf_map *map, size_t size, gfp_t flags)
{
	struct mem_cgroup *old_memcg;
	void *ptr;

	old_memcg = set_active_memcg(map->memcg);
	ptr = kzalloc(size, flags | __GFP_ACCOUNT);
	set_active_memcg(old_memcg);

	return ptr;
}