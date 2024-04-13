void *bpf_map_kmalloc_node(const struct bpf_map *map, size_t size, gfp_t flags,
			   int node)
{
	struct mem_cgroup *old_memcg;
	void *ptr;

	old_memcg = set_active_memcg(map->memcg);
	ptr = kmalloc_node(size, flags | __GFP_ACCOUNT, node);
	set_active_memcg(old_memcg);

	return ptr;
}