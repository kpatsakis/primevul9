static int bpf_for_each_hash_elem(struct bpf_map *map, void *callback_fn,
				  void *callback_ctx, u64 flags)
{
	struct bpf_htab *htab = container_of(map, struct bpf_htab, map);
	struct hlist_nulls_head *head;
	struct hlist_nulls_node *n;
	struct htab_elem *elem;
	u32 roundup_key_size;
	int i, num_elems = 0;
	void __percpu *pptr;
	struct bucket *b;
	void *key, *val;
	bool is_percpu;
	u64 ret = 0;

	if (flags != 0)
		return -EINVAL;

	is_percpu = htab_is_percpu(htab);

	roundup_key_size = round_up(map->key_size, 8);
	/* disable migration so percpu value prepared here will be the
	 * same as the one seen by the bpf program with bpf_map_lookup_elem().
	 */
	if (is_percpu)
		migrate_disable();
	for (i = 0; i < htab->n_buckets; i++) {
		b = &htab->buckets[i];
		rcu_read_lock();
		head = &b->head;
		hlist_nulls_for_each_entry_rcu(elem, n, head, hash_node) {
			key = elem->key;
			if (is_percpu) {
				/* current cpu value for percpu map */
				pptr = htab_elem_get_ptr(elem, map->key_size);
				val = this_cpu_ptr(pptr);
			} else {
				val = elem->key + roundup_key_size;
			}
			num_elems++;
			ret = BPF_CAST_CALL(callback_fn)((u64)(long)map,
					(u64)(long)key, (u64)(long)val,
					(u64)(long)callback_ctx, 0);
			/* return value: 0 - continue, 1 - stop and return */
			if (ret) {
				rcu_read_unlock();
				goto out;
			}
		}
		rcu_read_unlock();
	}
out:
	if (is_percpu)
		migrate_enable();
	return num_elems;
}