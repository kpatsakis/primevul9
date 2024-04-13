static void htab_elem_free(struct bpf_htab *htab, struct htab_elem *l)
{
	if (htab->map.map_type == BPF_MAP_TYPE_PERCPU_HASH)
		free_percpu(htab_elem_get_ptr(l, htab->map.key_size));
	kfree(l);
}