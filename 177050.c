static int push_node_left(struct btrfs_trans_handle *trans,
			  struct extent_buffer *dst,
			  struct extent_buffer *src, int empty)
{
	struct btrfs_fs_info *fs_info = trans->fs_info;
	int push_items = 0;
	int src_nritems;
	int dst_nritems;
	int ret = 0;

	src_nritems = btrfs_header_nritems(src);
	dst_nritems = btrfs_header_nritems(dst);
	push_items = BTRFS_NODEPTRS_PER_BLOCK(fs_info) - dst_nritems;
	WARN_ON(btrfs_header_generation(src) != trans->transid);
	WARN_ON(btrfs_header_generation(dst) != trans->transid);

	if (!empty && src_nritems <= 8)
		return 1;

	if (push_items <= 0)
		return 1;

	if (empty) {
		push_items = min(src_nritems, push_items);
		if (push_items < src_nritems) {
			/* leave at least 8 pointers in the node if
			 * we aren't going to empty it
			 */
			if (src_nritems - push_items < 8) {
				if (push_items <= 8)
					return 1;
				push_items -= 8;
			}
		}
	} else
		push_items = min(src_nritems - 8, push_items);

	/* dst is the left eb, src is the middle eb */
	if (check_sibling_keys(dst, src)) {
		ret = -EUCLEAN;
		btrfs_abort_transaction(trans, ret);
		return ret;
	}
	ret = tree_mod_log_eb_copy(dst, src, dst_nritems, 0, push_items);
	if (ret) {
		btrfs_abort_transaction(trans, ret);
		return ret;
	}
	copy_extent_buffer(dst, src,
			   btrfs_node_key_ptr_offset(dst_nritems),
			   btrfs_node_key_ptr_offset(0),
			   push_items * sizeof(struct btrfs_key_ptr));

	if (push_items < src_nritems) {
		/*
		 * Don't call tree_mod_log_insert_move here, key removal was
		 * already fully logged by tree_mod_log_eb_copy above.
		 */
		memmove_extent_buffer(src, btrfs_node_key_ptr_offset(0),
				      btrfs_node_key_ptr_offset(push_items),
				      (src_nritems - push_items) *
				      sizeof(struct btrfs_key_ptr));
	}
	btrfs_set_header_nritems(src, src_nritems - push_items);
	btrfs_set_header_nritems(dst, dst_nritems + push_items);
	btrfs_mark_buffer_dirty(src);
	btrfs_mark_buffer_dirty(dst);

	return ret;
}