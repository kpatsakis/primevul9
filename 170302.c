static void bpf_map_save_memcg(struct bpf_map *map)
{
	map->memcg = get_mem_cgroup_from_mm(current->mm);
}