void __percpu *bpf_map_alloc_percpu(const struct bpf_map *map, size_t size,
				    size_t align, gfp_t flags)
{
	struct mem_cgroup *old_memcg;
	void __percpu *ptr;

	old_memcg = set_active_memcg(map->memcg);
	ptr = __alloc_percpu_gfp(size, align, flags | __GFP_ACCOUNT);
	set_active_memcg(old_memcg);

	return ptr;
}