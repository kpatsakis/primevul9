static int __htab_lru_percpu_map_update_elem(struct bpf_map *map, void *key,
					     void *value, u64 map_flags,
					     bool onallcpus)
{
	struct bpf_htab *htab = container_of(map, struct bpf_htab, map);
	struct htab_elem *l_new = NULL, *l_old;
	struct hlist_nulls_head *head;
	unsigned long flags;
	struct bucket *b;
	u32 key_size, hash;
	int ret;

	if (unlikely(map_flags > BPF_EXIST))
		/* unknown flags */
		return -EINVAL;

	WARN_ON_ONCE(!rcu_read_lock_held() && !rcu_read_lock_trace_held() &&
		     !rcu_read_lock_bh_held());

	key_size = map->key_size;

	hash = htab_map_hash(key, key_size, htab->hashrnd);

	b = __select_bucket(htab, hash);
	head = &b->head;

	/* For LRU, we need to alloc before taking bucket's
	 * spinlock because LRU's elem alloc may need
	 * to remove older elem from htab and this removal
	 * operation will need a bucket lock.
	 */
	if (map_flags != BPF_EXIST) {
		l_new = prealloc_lru_pop(htab, key, hash);
		if (!l_new)
			return -ENOMEM;
	}

	ret = htab_lock_bucket(htab, b, hash, &flags);
	if (ret)
		return ret;

	l_old = lookup_elem_raw(head, hash, key, key_size);

	ret = check_flags(htab, l_old, map_flags);
	if (ret)
		goto err;

	if (l_old) {
		bpf_lru_node_set_ref(&l_old->lru_node);

		/* per-cpu hash map can update value in-place */
		pcpu_copy_value(htab, htab_elem_get_ptr(l_old, key_size),
				value, onallcpus);
	} else {
		pcpu_init_value(htab, htab_elem_get_ptr(l_new, key_size),
				value, onallcpus);
		hlist_nulls_add_head_rcu(&l_new->hash_node, head);
		l_new = NULL;
	}
	ret = 0;
err:
	htab_unlock_bucket(htab, b, hash, flags);
	if (l_new)
		bpf_lru_push_free(&htab->lru, &l_new->lru_node);
	return ret;
}