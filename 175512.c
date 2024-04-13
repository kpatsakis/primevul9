static void fd_htab_map_free(struct bpf_map *map)
{
	struct bpf_htab *htab = container_of(map, struct bpf_htab, map);
	struct hlist_nulls_node *n;
	struct hlist_nulls_head *head;
	struct htab_elem *l;
	int i;

	for (i = 0; i < htab->n_buckets; i++) {
		head = select_bucket(htab, i);

		hlist_nulls_for_each_entry_safe(l, n, head, hash_node) {
			void *ptr = fd_htab_map_get_ptr(map, l);

			map->ops->map_fd_put_ptr(ptr);
		}
	}

	htab_map_free(map);
}