int btrfs_copy_root(struct btrfs_trans_handle *trans,
		      struct btrfs_root *root,
		      struct extent_buffer *buf,
		      struct extent_buffer **cow_ret, u64 new_root_objectid)
{
	struct btrfs_fs_info *fs_info = root->fs_info;
	struct extent_buffer *cow;
	int ret = 0;
	int level;
	struct btrfs_disk_key disk_key;

	WARN_ON(test_bit(BTRFS_ROOT_SHAREABLE, &root->state) &&
		trans->transid != fs_info->running_transaction->transid);
	WARN_ON(test_bit(BTRFS_ROOT_SHAREABLE, &root->state) &&
		trans->transid != root->last_trans);

	level = btrfs_header_level(buf);
	if (level == 0)
		btrfs_item_key(buf, &disk_key, 0);
	else
		btrfs_node_key(buf, &disk_key, 0);

	cow = btrfs_alloc_tree_block(trans, root, 0, new_root_objectid,
				     &disk_key, level, buf->start, 0,
				     BTRFS_NESTING_NEW_ROOT);
	if (IS_ERR(cow))
		return PTR_ERR(cow);

	copy_extent_buffer_full(cow, buf);
	btrfs_set_header_bytenr(cow, cow->start);
	btrfs_set_header_generation(cow, trans->transid);
	btrfs_set_header_backref_rev(cow, BTRFS_MIXED_BACKREF_REV);
	btrfs_clear_header_flag(cow, BTRFS_HEADER_FLAG_WRITTEN |
				     BTRFS_HEADER_FLAG_RELOC);
	if (new_root_objectid == BTRFS_TREE_RELOC_OBJECTID)
		btrfs_set_header_flag(cow, BTRFS_HEADER_FLAG_RELOC);
	else
		btrfs_set_header_owner(cow, new_root_objectid);

	write_extent_buffer_fsid(cow, fs_info->fs_devices->metadata_uuid);

	WARN_ON(btrfs_header_generation(buf) > trans->transid);
	if (new_root_objectid == BTRFS_TREE_RELOC_OBJECTID)
		ret = btrfs_inc_ref(trans, root, cow, 1);
	else
		ret = btrfs_inc_ref(trans, root, cow, 0);
	if (ret) {
		btrfs_tree_unlock(cow);
		free_extent_buffer(cow);
		btrfs_abort_transaction(trans, ret);
		return ret;
	}

	btrfs_mark_buffer_dirty(cow);
	*cow_ret = cow;
	return 0;
}