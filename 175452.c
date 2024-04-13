static int __htab_map_lookup_and_delete_elem(struct bpf_map *map, void *key,
					     void *value, bool is_lru_map,
					     bool is_percpu, u64 flags)
{
	struct bpf_htab *htab = container_of(map, struct bpf_htab, map);
	struct hlist_nulls_head *head;
	unsigned long bflags;
	struct htab_elem *l;
	u32 hash, key_size;
	struct bucket *b;
	int ret;

	key_size = map->key_size;

	hash = htab_map_hash(key, key_size, htab->hashrnd);
	b = __select_bucket(htab, hash);
	head = &b->head;

	ret = htab_lock_bucket(htab, b, hash, &bflags);
	if (ret)
		return ret;

	l = lookup_elem_raw(head, hash, key, key_size);
	if (!l) {
		ret = -ENOENT;
	} else {
		if (is_percpu) {
			u32 roundup_value_size = round_up(map->value_size, 8);
			void __percpu *pptr;
			int off = 0, cpu;

			pptr = htab_elem_get_ptr(l, key_size);
			for_each_possible_cpu(cpu) {
				bpf_long_memcpy(value + off,
						per_cpu_ptr(pptr, cpu),
						roundup_value_size);
				off += roundup_value_size;
			}
		} else {
			u32 roundup_key_size = round_up(map->key_size, 8);

			if (flags & BPF_F_LOCK)
				copy_map_value_locked(map, value, l->key +
						      roundup_key_size,
						      true);
			else
				copy_map_value(map, value, l->key +
					       roundup_key_size);
			check_and_init_map_lock(map, value);
		}

		hlist_nulls_del_rcu(&l->hash_node);
		if (!is_lru_map)
			free_htab_elem(htab, l);
	}

	htab_unlock_bucket(htab, b, hash, bflags);

	if (is_lru_map && l)
		bpf_lru_push_free(&htab->lru, &l->lru_node);

	return ret;
}