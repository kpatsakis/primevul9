static struct htab_elem *alloc_htab_elem(struct bpf_htab *htab, void *key,
					 void *value, u32 key_size, u32 hash,
					 bool percpu, bool onallcpus,
					 struct htab_elem *old_elem)
{
	u32 size = htab->map.value_size;
	bool prealloc = htab_is_prealloc(htab);
	struct htab_elem *l_new, **pl_new;
	void __percpu *pptr;

	if (prealloc) {
		if (old_elem) {
			/* if we're updating the existing element,
			 * use per-cpu extra elems to avoid freelist_pop/push
			 */
			pl_new = this_cpu_ptr(htab->extra_elems);
			l_new = *pl_new;
			htab_put_fd_value(htab, old_elem);
			*pl_new = old_elem;
		} else {
			struct pcpu_freelist_node *l;

			l = __pcpu_freelist_pop(&htab->freelist);
			if (!l)
				return ERR_PTR(-E2BIG);
			l_new = container_of(l, struct htab_elem, fnode);
		}
	} else {
		if (atomic_inc_return(&htab->count) > htab->map.max_entries)
			if (!old_elem) {
				/* when map is full and update() is replacing
				 * old element, it's ok to allocate, since
				 * old element will be freed immediately.
				 * Otherwise return an error
				 */
				l_new = ERR_PTR(-E2BIG);
				goto dec_count;
			}
		l_new = bpf_map_kmalloc_node(&htab->map, htab->elem_size,
					     GFP_ATOMIC | __GFP_NOWARN,
					     htab->map.numa_node);
		if (!l_new) {
			l_new = ERR_PTR(-ENOMEM);
			goto dec_count;
		}
		check_and_init_map_lock(&htab->map,
					l_new->key + round_up(key_size, 8));
	}

	memcpy(l_new->key, key, key_size);
	if (percpu) {
		size = round_up(size, 8);
		if (prealloc) {
			pptr = htab_elem_get_ptr(l_new, key_size);
		} else {
			/* alloc_percpu zero-fills */
			pptr = bpf_map_alloc_percpu(&htab->map, size, 8,
						    GFP_ATOMIC | __GFP_NOWARN);
			if (!pptr) {
				kfree(l_new);
				l_new = ERR_PTR(-ENOMEM);
				goto dec_count;
			}
		}

		pcpu_init_value(htab, pptr, value, onallcpus);

		if (!prealloc)
			htab_elem_set_ptr(l_new, key_size, pptr);
	} else if (fd_htab_map_needs_adjust(htab)) {
		size = round_up(size, 8);
		memcpy(l_new->key + round_up(key_size, 8), value, size);
	} else {
		copy_map_value(&htab->map,
			       l_new->key + round_up(key_size, 8),
			       value);
	}

	l_new->hash = hash;
	return l_new;
dec_count:
	atomic_dec(&htab->count);
	return l_new;
}