static noinline int __btrfs_cow_block(struct btrfs_trans_handle *trans,
			     struct btrfs_root *root,
			     struct extent_buffer *buf,
			     struct extent_buffer *parent, int parent_slot,
			     struct extent_buffer **cow_ret,
			     u64 search_start, u64 empty_size,
			     enum btrfs_lock_nesting nest)
{
	struct btrfs_fs_info *fs_info = root->fs_info;
	struct btrfs_disk_key disk_key;
	struct extent_buffer *cow;
	int level, ret;
	int last_ref = 0;
	int unlock_orig = 0;
	u64 parent_start = 0;

	if (*cow_ret == buf)
		unlock_orig = 1;

	btrfs_assert_tree_locked(buf);

	WARN_ON(test_bit(BTRFS_ROOT_SHAREABLE, &root->state) &&
		trans->transid != fs_info->running_transaction->transid);
	WARN_ON(test_bit(BTRFS_ROOT_SHAREABLE, &root->state) &&
		trans->transid != root->last_trans);

	level = btrfs_header_level(buf);

	if (level == 0)
		btrfs_item_key(buf, &disk_key, 0);
	else
		btrfs_node_key(buf, &disk_key, 0);

	if ((root->root_key.objectid == BTRFS_TREE_RELOC_OBJECTID) && parent)
		parent_start = parent->start;

	cow = alloc_tree_block_no_bg_flush(trans, root, parent_start, &disk_key,
					   level, search_start, empty_size, nest);
	if (IS_ERR(cow))
		return PTR_ERR(cow);

	/* cow is set to blocking by btrfs_init_new_buffer */

	copy_extent_buffer_full(cow, buf);
	btrfs_set_header_bytenr(cow, cow->start);
	btrfs_set_header_generation(cow, trans->transid);
	btrfs_set_header_backref_rev(cow, BTRFS_MIXED_BACKREF_REV);
	btrfs_clear_header_flag(cow, BTRFS_HEADER_FLAG_WRITTEN |
				     BTRFS_HEADER_FLAG_RELOC);
	if (root->root_key.objectid == BTRFS_TREE_RELOC_OBJECTID)
		btrfs_set_header_flag(cow, BTRFS_HEADER_FLAG_RELOC);
	else
		btrfs_set_header_owner(cow, root->root_key.objectid);

	write_extent_buffer_fsid(cow, fs_info->fs_devices->metadata_uuid);

	ret = update_ref_for_cow(trans, root, buf, cow, &last_ref);
	if (ret) {
		btrfs_tree_unlock(cow);
		free_extent_buffer(cow);
		btrfs_abort_transaction(trans, ret);
		return ret;
	}

	if (test_bit(BTRFS_ROOT_SHAREABLE, &root->state)) {
		ret = btrfs_reloc_cow_block(trans, root, buf, cow);
		if (ret) {
			btrfs_tree_unlock(cow);
			free_extent_buffer(cow);
			btrfs_abort_transaction(trans, ret);
			return ret;
		}
	}

	if (buf == root->node) {
		WARN_ON(parent && parent != buf);
		if (root->root_key.objectid == BTRFS_TREE_RELOC_OBJECTID ||
		    btrfs_header_backref_rev(buf) < BTRFS_MIXED_BACKREF_REV)
			parent_start = buf->start;

		atomic_inc(&cow->refs);
		ret = tree_mod_log_insert_root(root->node, cow, 1);
		BUG_ON(ret < 0);
		rcu_assign_pointer(root->node, cow);

		btrfs_free_tree_block(trans, root, buf, parent_start,
				      last_ref);
		free_extent_buffer(buf);
		add_root_to_dirty_list(root);
	} else {
		WARN_ON(trans->transid != btrfs_header_generation(parent));
		tree_mod_log_insert_key(parent, parent_slot,
					MOD_LOG_KEY_REPLACE, GFP_NOFS);
		btrfs_set_node_blockptr(parent, parent_slot,
					cow->start);
		btrfs_set_node_ptr_generation(parent, parent_slot,
					      trans->transid);
		btrfs_mark_buffer_dirty(parent);
		if (last_ref) {
			ret = tree_mod_log_free_eb(buf);
			if (ret) {
				btrfs_tree_unlock(cow);
				free_extent_buffer(cow);
				btrfs_abort_transaction(trans, ret);
				return ret;
			}
		}
		btrfs_free_tree_block(trans, root, buf, parent_start,
				      last_ref);
	}
	if (unlock_orig)
		btrfs_tree_unlock(buf);
	free_extent_buffer_stale(buf);
	btrfs_mark_buffer_dirty(cow);
	*cow_ret = cow;
	return 0;
}