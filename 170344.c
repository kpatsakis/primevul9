int map_check_no_btf(const struct bpf_map *map,
		     const struct btf *btf,
		     const struct btf_type *key_type,
		     const struct btf_type *value_type)
{
	return -ENOTSUPP;
}