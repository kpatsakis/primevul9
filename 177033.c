__tree_mod_log_rewind(struct btrfs_fs_info *fs_info, struct extent_buffer *eb,
		      u64 time_seq, struct tree_mod_elem *first_tm)
{
	u32 n;
	struct rb_node *next;
	struct tree_mod_elem *tm = first_tm;
	unsigned long o_dst;
	unsigned long o_src;
	unsigned long p_size = sizeof(struct btrfs_key_ptr);

	n = btrfs_header_nritems(eb);
	read_lock(&fs_info->tree_mod_log_lock);
	while (tm && tm->seq >= time_seq) {
		/*
		 * all the operations are recorded with the operator used for
		 * the modification. as we're going backwards, we do the
		 * opposite of each operation here.
		 */
		switch (tm->op) {
		case MOD_LOG_KEY_REMOVE_WHILE_FREEING:
			BUG_ON(tm->slot < n);
			fallthrough;
		case MOD_LOG_KEY_REMOVE_WHILE_MOVING:
		case MOD_LOG_KEY_REMOVE:
			btrfs_set_node_key(eb, &tm->key, tm->slot);
			btrfs_set_node_blockptr(eb, tm->slot, tm->blockptr);
			btrfs_set_node_ptr_generation(eb, tm->slot,
						      tm->generation);
			n++;
			break;
		case MOD_LOG_KEY_REPLACE:
			BUG_ON(tm->slot >= n);
			btrfs_set_node_key(eb, &tm->key, tm->slot);
			btrfs_set_node_blockptr(eb, tm->slot, tm->blockptr);
			btrfs_set_node_ptr_generation(eb, tm->slot,
						      tm->generation);
			break;
		case MOD_LOG_KEY_ADD:
			/* if a move operation is needed it's in the log */
			n--;
			break;
		case MOD_LOG_MOVE_KEYS:
			o_dst = btrfs_node_key_ptr_offset(tm->slot);
			o_src = btrfs_node_key_ptr_offset(tm->move.dst_slot);
			memmove_extent_buffer(eb, o_dst, o_src,
					      tm->move.nr_items * p_size);
			break;
		case MOD_LOG_ROOT_REPLACE:
			/*
			 * this operation is special. for roots, this must be
			 * handled explicitly before rewinding.
			 * for non-roots, this operation may exist if the node
			 * was a root: root A -> child B; then A gets empty and
			 * B is promoted to the new root. in the mod log, we'll
			 * have a root-replace operation for B, a tree block
			 * that is no root. we simply ignore that operation.
			 */
			break;
		}
		next = rb_next(&tm->node);
		if (!next)
			break;
		tm = rb_entry(next, struct tree_mod_elem, node);
		if (tm->logical != first_tm->logical)
			break;
	}
	read_unlock(&fs_info->tree_mod_log_lock);
	btrfs_set_header_nritems(eb, n);
}