bool bpf_map_meta_equal(const struct bpf_map *meta0,
			const struct bpf_map *meta1)
{
	/* No need to compare ops because it is covered by map_type */
	return meta0->map_type == meta1->map_type &&
		meta0->key_size == meta1->key_size &&
		meta0->value_size == meta1->value_size &&
		meta0->map_flags == meta1->map_flags &&
		meta0->max_entries == meta1->max_entries;
}