static __always_inline void *__htab_lru_map_lookup_elem(struct bpf_map *map,
							void *key, const bool mark)
{
	struct htab_elem *l = __htab_map_lookup_elem(map, key);

	if (l) {
		if (mark)
			bpf_lru_node_set_ref(&l->lru_node);
		return l->key + round_up(map->key_size, 8);
	}

	return NULL;
}