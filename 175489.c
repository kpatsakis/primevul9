static void free_htab_elem(struct bpf_htab *htab, struct htab_elem *l)
{
	htab_put_fd_value(htab, l);

	if (htab_is_prealloc(htab)) {
		__pcpu_freelist_push(&htab->freelist, &l->fnode);
	} else {
		atomic_dec(&htab->count);
		l->htab = htab;
		call_rcu(&l->rcu, htab_elem_free_rcu);
	}
}