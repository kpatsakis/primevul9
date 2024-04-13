static bool htab_lru_map_delete_node(void *arg, struct bpf_lru_node *node)
{
	struct bpf_htab *htab = (struct bpf_htab *)arg;
	struct htab_elem *l = NULL, *tgt_l;
	struct hlist_nulls_head *head;
	struct hlist_nulls_node *n;
	unsigned long flags;
	struct bucket *b;
	int ret;

	tgt_l = container_of(node, struct htab_elem, lru_node);
	b = __select_bucket(htab, tgt_l->hash);
	head = &b->head;

	ret = htab_lock_bucket(htab, b, tgt_l->hash, &flags);
	if (ret)
		return false;

	hlist_nulls_for_each_entry_rcu(l, n, head, hash_node)
		if (l == tgt_l) {
			hlist_nulls_del_rcu(&l->hash_node);
			break;
		}

	htab_unlock_bucket(htab, b, tgt_l->hash, flags);

	return l == tgt_l;
}