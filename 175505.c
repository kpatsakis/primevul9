static struct htab_elem *lookup_nulls_elem_raw(struct hlist_nulls_head *head,
					       u32 hash, void *key,
					       u32 key_size, u32 n_buckets)
{
	struct hlist_nulls_node *n;
	struct htab_elem *l;

again:
	hlist_nulls_for_each_entry_rcu(l, n, head, hash_node)
		if (l->hash == hash && !memcmp(&l->key, key, key_size))
			return l;

	if (unlikely(get_nulls_value(n) != (hash & (n_buckets - 1))))
		goto again;

	return NULL;
}