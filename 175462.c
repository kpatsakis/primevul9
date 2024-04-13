static void delete_all_elements(struct bpf_htab *htab)
{
	int i;

	for (i = 0; i < htab->n_buckets; i++) {
		struct hlist_nulls_head *head = select_bucket(htab, i);
		struct hlist_nulls_node *n;
		struct htab_elem *l;

		hlist_nulls_for_each_entry_safe(l, n, head, hash_node) {
			hlist_nulls_del_rcu(&l->hash_node);
			htab_elem_free(htab, l);
		}
	}
}