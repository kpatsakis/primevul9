static int htab_lru_map_update_elem(struct bpf_map *map, void *key, void *value,
				    u64 map_flags)
{
	struct bpf_htab *htab = container_of(map, struct bpf_htab, map);
	struct htab_elem *l_new, *l_old = NULL;
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
	 * spinlock because getting free nodes from LRU may need
	 * to remove older elements from htab and this removal
	 * operation will need a bucket lock.
	 */
	l_new = prealloc_lru_pop(htab, key, hash);
	if (!l_new)
		return -ENOMEM;
	memcpy(l_new->key + round_up(map->key_size, 8), value, map->value_size);

	ret = htab_lock_bucket(htab, b, hash, &flags);
	if (ret)
		return ret;

	l_old = lookup_elem_raw(head, hash, key, key_size);

	ret = check_flags(htab, l_old, map_flags);
	if (ret)
		goto err;

	/* add new element to the head of the list, so that
	 * concurrent search will find it before old elem
	 */
	hlist_nulls_add_head_rcu(&l_new->hash_node, head);
	if (l_old) {
		bpf_lru_node_set_ref(&l_new->lru_node);
		hlist_nulls_del_rcu(&l_old->hash_node);
	}
	ret = 0;

err:
	htab_unlock_bucket(htab, b, hash, flags);

	if (ret)
		bpf_lru_push_free(&htab->lru, &l_new->lru_node);
	else if (l_old)
		bpf_lru_push_free(&htab->lru, &l_old->lru_node);

	return ret;
}