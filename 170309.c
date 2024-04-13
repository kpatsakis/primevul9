int __weak bpf_stackmap_copy(struct bpf_map *map, void *key, void *value)
{
	return -ENOTSUPP;
}