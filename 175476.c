bpf_hash_map_seq_find_next(struct bpf_iter_seq_hash_map_info *info,
			   struct htab_elem *prev_elem)
{
	const struct bpf_htab *htab = info->htab;
	u32 skip_elems = info->skip_elems;
	u32 bucket_id = info->bucket_id;
	struct hlist_nulls_head *head;
	struct hlist_nulls_node *n;
	struct htab_elem *elem;
	struct bucket *b;
	u32 i, count;

	if (bucket_id >= htab->n_buckets)
		return NULL;

	/* try to find next elem in the same bucket */
	if (prev_elem) {
		/* no update/deletion on this bucket, prev_elem should be still valid
		 * and we won't skip elements.
		 */
		n = rcu_dereference_raw(hlist_nulls_next_rcu(&prev_elem->hash_node));
		elem = hlist_nulls_entry_safe(n, struct htab_elem, hash_node);
		if (elem)
			return elem;

		/* not found, unlock and go to the next bucket */
		b = &htab->buckets[bucket_id++];
		rcu_read_unlock();
		skip_elems = 0;
	}

	for (i = bucket_id; i < htab->n_buckets; i++) {
		b = &htab->buckets[i];
		rcu_read_lock();

		count = 0;
		head = &b->head;
		hlist_nulls_for_each_entry_rcu(elem, n, head, hash_node) {
			if (count >= skip_elems) {
				info->bucket_id = i;
				info->skip_elems = count;
				return elem;
			}
			count++;
		}

		rcu_read_unlock();
		skip_elems = 0;
	}

	info->bucket_id = i;
	info->skip_elems = 0;
	return NULL;
}