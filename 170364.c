void *bpf_map_area_mmapable_alloc(u64 size, int numa_node)
{
	return __bpf_map_area_alloc(size, numa_node, true);
}