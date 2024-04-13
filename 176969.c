static int balance_node_right(struct btrfs_trans_handle *trans,
			      struct extent_buffer *dst,
			      struct extent_buffer *src)
{
	struct btrfs_fs_info *fs_info = trans->fs_info;
	int push_items = 0;
	int max_push;
	int src_nritems;
	int dst_nritems;
	int ret = 0;

	WARN_ON(btrfs_header_generation(src) != trans->transid);
	WARN_ON(btrfs_header_generation(dst) != trans->transid);

	src_nritems = btrfs_header_nritems(src);
	dst_nritems = btrfs_header_nritems(dst);
	push_items = BTRFS_NODEPTRS_PER_BLOCK(fs_info) - dst_nritems;
	if (push_items <= 0)
		return 1;

	if (src_nritems < 4)
		return 1;

	max_push = src_nritems / 2 + 1;
	/* don't try to empty the node */
	if (max_push >= src_nritems)
		return 1;

	if (max_push < push_items)
		push_items = max_push;

	/* dst is the right eb, src is the middle eb */
	if (check_sibling_keys(src, dst)) {
		ret = -EUCLEAN;
		btrfs_abort_transaction(trans, ret);
		return ret;
	}
	ret = tree_mod_log_insert_move(dst, push_items, 0, dst_nritems);
	BUG_ON(ret < 0);
	memmove_extent_buffer(dst, btrfs_node_key_ptr_offset(push_items),
				      btrfs_node_key_ptr_offset(0),
				      (dst_nritems) *
				      sizeof(struct btrfs_key_ptr));

	ret = tree_mod_log_eb_copy(dst, src, 0, src_nritems - push_items,
				   push_items);
	if (ret) {
		btrfs_abort_transaction(trans, ret);
		return ret;
	}
	copy_extent_buffer(dst, src,
			   btrfs_node_key_ptr_offset(0),
			   btrfs_node_key_ptr_offset(src_nritems - push_items),
			   push_items * sizeof(struct btrfs_key_ptr));

	btrfs_set_header_nritems(src, src_nritems - push_items);
	btrfs_set_header_nritems(dst, dst_nritems + push_items);

	btrfs_mark_buffer_dirty(src);
	btrfs_mark_buffer_dirty(dst);

	return ret;
}