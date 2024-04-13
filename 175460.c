static int prealloc_init(struct bpf_htab *htab)
{
	u32 num_entries = htab->map.max_entries;
	int err = -ENOMEM, i;

	if (!htab_is_percpu(htab) && !htab_is_lru(htab))
		num_entries += num_possible_cpus();

	htab->elems = bpf_map_area_alloc((u64)htab->elem_size * num_entries,
					 htab->map.numa_node);
	if (!htab->elems)
		return -ENOMEM;

	if (!htab_is_percpu(htab))
		goto skip_percpu_elems;

	for (i = 0; i < num_entries; i++) {
		u32 size = round_up(htab->map.value_size, 8);
		void __percpu *pptr;

		pptr = bpf_map_alloc_percpu(&htab->map, size, 8,
					    GFP_USER | __GFP_NOWARN);
		if (!pptr)
			goto free_elems;
		htab_elem_set_ptr(get_htab_elem(htab, i), htab->map.key_size,
				  pptr);
		cond_resched();
	}

skip_percpu_elems:
	if (htab_is_lru(htab))
		err = bpf_lru_init(&htab->lru,
				   htab->map.map_flags & BPF_F_NO_COMMON_LRU,
				   offsetof(struct htab_elem, hash) -
				   offsetof(struct htab_elem, lru_node),
				   htab_lru_map_delete_node,
				   htab);
	else
		err = pcpu_freelist_init(&htab->freelist);

	if (err)
		goto free_elems;

	if (htab_is_lru(htab))
		bpf_lru_populate(&htab->lru, htab->elems,
				 offsetof(struct htab_elem, lru_node),
				 htab->elem_size, num_entries);
	else
		pcpu_freelist_populate(&htab->freelist,
				       htab->elems + offsetof(struct htab_elem, fnode),
				       htab->elem_size, num_entries);

	return 0;

free_elems:
	htab_free_elems(htab);
	return err;
}