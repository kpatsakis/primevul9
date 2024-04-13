htab_lru_map_lookup_and_delete_batch(struct bpf_map *map,
				     const union bpf_attr *attr,
				     union bpf_attr __user *uattr)
{
	return __htab_map_lookup_and_delete_batch(map, attr, uattr, true,
						  true, false);
}