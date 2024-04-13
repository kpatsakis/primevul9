static void htab_free_elems(struct bpf_htab *htab)
{
	int i;

	if (!htab_is_percpu(htab))
		goto free_elems;

	for (i = 0; i < htab->map.max_entries; i++) {
		void __percpu *pptr;

		pptr = htab_elem_get_ptr(get_htab_elem(htab, i),
					 htab->map.key_size);
		free_percpu(pptr);
		cond_resched();
	}
free_elems:
	bpf_map_area_free(htab->elems);
}