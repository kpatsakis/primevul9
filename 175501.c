static void prealloc_destroy(struct bpf_htab *htab)
{
	htab_free_elems(htab);

	if (htab_is_lru(htab))
		bpf_lru_destroy(&htab->lru);
	else
		pcpu_freelist_destroy(&htab->freelist);
}