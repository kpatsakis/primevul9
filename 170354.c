static void bpf_map_release_memcg(struct bpf_map *map)
{
	mem_cgroup_put(map->memcg);
}