static int __htab_percpu_map_update_elem(struct bpf_map *map, void *key,
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

	ret = htab_lock_bucket(htab, b, hash, &flags);
	if (ret)
		return ret;

	l_old = lookup_elem_raw(head, hash, key, key_size);

	ret = check_flags(htab, l_old, map_flags);
	if (ret)
		goto err;

	if (l_old) {
		/* per-cpu hash map can update value in-place */
		pcpu_copy_value(htab, htab_elem_get_ptr(l_old, key_size),
				value, onallcpus);
	} else {
		l_new = alloc_htab_elem(htab, key, value, key_size,
					hash, true, onallcpus, NULL);
		if (IS_ERR(l_new)) {
			ret = PTR_ERR(l_new);
			goto err;
		}
		hlist_nulls_add_head_rcu(&l_new->hash_node, head);
	}
	ret = 0;
err:
	htab_unlock_bucket(htab, b, hash, flags);
	return ret;
}