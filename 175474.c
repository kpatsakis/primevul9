static bool htab_is_percpu(const struct bpf_htab *htab)
{
	return htab->map.map_type == BPF_MAP_TYPE_PERCPU_HASH ||
		htab->map.map_type == BPF_MAP_TYPE_LRU_PERCPU_HASH;
}