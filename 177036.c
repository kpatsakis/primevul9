alloc_tree_mod_elem(struct extent_buffer *eb, int slot,
		    enum mod_log_op op, gfp_t flags)
{
	struct tree_mod_elem *tm;

	tm = kzalloc(sizeof(*tm), flags);
	if (!tm)
		return NULL;

	tm->logical = eb->start;
	if (op != MOD_LOG_KEY_ADD) {
		btrfs_node_key(eb, &tm->key, slot);
		tm->blockptr = btrfs_node_blockptr(eb, slot);
	}
	tm->op = op;
	tm->slot = slot;
	tm->generation = btrfs_node_ptr_generation(eb, slot);
	RB_CLEAR_NODE(&tm->node);

	return tm;
}