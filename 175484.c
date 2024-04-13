static struct bpf_map *htab_map_alloc(union bpf_attr *attr)
{
	bool percpu = (attr->map_type == BPF_MAP_TYPE_PERCPU_HASH ||
		       attr->map_type == BPF_MAP_TYPE_LRU_PERCPU_HASH);
	bool lru = (attr->map_type == BPF_MAP_TYPE_LRU_HASH ||
		    attr->map_type == BPF_MAP_TYPE_LRU_PERCPU_HASH);
	/* percpu_lru means each cpu has its own LRU list.
	 * it is different from BPF_MAP_TYPE_PERCPU_HASH where
	 * the map's value itself is percpu.  percpu_lru has
	 * nothing to do with the map's value.
	 */
	bool percpu_lru = (attr->map_flags & BPF_F_NO_COMMON_LRU);
	bool prealloc = !(attr->map_flags & BPF_F_NO_PREALLOC);
	struct bpf_htab *htab;
	int err, i;

	htab = kzalloc(sizeof(*htab), GFP_USER | __GFP_ACCOUNT);
	if (!htab)
		return ERR_PTR(-ENOMEM);

	lockdep_register_key(&htab->lockdep_key);

	bpf_map_init_from_attr(&htab->map, attr);

	if (percpu_lru) {
		/* ensure each CPU's lru list has >=1 elements.
		 * since we are at it, make each lru list has the same
		 * number of elements.
		 */
		htab->map.max_entries = roundup(attr->max_entries,
						num_possible_cpus());
		if (htab->map.max_entries < attr->max_entries)
			htab->map.max_entries = rounddown(attr->max_entries,
							  num_possible_cpus());
	}

	/* hash table size must be power of 2 */
	htab->n_buckets = roundup_pow_of_two(htab->map.max_entries);

	htab->elem_size = sizeof(struct htab_elem) +
			  round_up(htab->map.key_size, 8);
	if (percpu)
		htab->elem_size += sizeof(void *);
	else
		htab->elem_size += round_up(htab->map.value_size, 8);

	err = -E2BIG;
	/* prevent zero size kmalloc and check for u32 overflow */
	if (htab->n_buckets == 0 ||
	    htab->n_buckets > U32_MAX / sizeof(struct bucket))
		goto free_htab;

	err = -ENOMEM;
	htab->buckets = bpf_map_area_alloc(htab->n_buckets *
					   sizeof(struct bucket),
					   htab->map.numa_node);
	if (!htab->buckets)
		goto free_htab;

	for (i = 0; i < HASHTAB_MAP_LOCK_COUNT; i++) {
		htab->map_locked[i] = bpf_map_alloc_percpu(&htab->map,
							   sizeof(int),
							   sizeof(int),
							   GFP_USER);
		if (!htab->map_locked[i])
			goto free_map_locked;
	}

	if (htab->map.map_flags & BPF_F_ZERO_SEED)
		htab->hashrnd = 0;
	else
		htab->hashrnd = get_random_int();

	htab_init_buckets(htab);

	if (prealloc) {
		err = prealloc_init(htab);
		if (err)
			goto free_map_locked;

		if (!percpu && !lru) {
			/* lru itself can remove the least used element, so
			 * there is no need for an extra elem during map_update.
			 */
			err = alloc_extra_elems(htab);
			if (err)
				goto free_prealloc;
		}
	}

	return &htab->map;

free_prealloc:
	prealloc_destroy(htab);
free_map_locked:
	for (i = 0; i < HASHTAB_MAP_LOCK_COUNT; i++)
		free_percpu(htab->map_locked[i]);
	bpf_map_area_free(htab->buckets);
free_htab:
	lockdep_unregister_key(&htab->lockdep_key);
	kfree(htab);
	return ERR_PTR(err);
}