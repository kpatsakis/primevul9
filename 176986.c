int btrfs_realloc_node(struct btrfs_trans_handle *trans,
		       struct btrfs_root *root, struct extent_buffer *parent,
		       int start_slot, u64 *last_ret,
		       struct btrfs_key *progress)
{
	struct btrfs_fs_info *fs_info = root->fs_info;
	struct extent_buffer *cur;
	u64 blocknr;
	u64 search_start = *last_ret;
	u64 last_block = 0;
	u64 other;
	u32 parent_nritems;
	int end_slot;
	int i;
	int err = 0;
	u32 blocksize;
	int progress_passed = 0;
	struct btrfs_disk_key disk_key;

	WARN_ON(trans->transaction != fs_info->running_transaction);
	WARN_ON(trans->transid != fs_info->generation);

	parent_nritems = btrfs_header_nritems(parent);
	blocksize = fs_info->nodesize;
	end_slot = parent_nritems - 1;

	if (parent_nritems <= 1)
		return 0;

	for (i = start_slot; i <= end_slot; i++) {
		int close = 1;

		btrfs_node_key(parent, &disk_key, i);
		if (!progress_passed && comp_keys(&disk_key, progress) < 0)
			continue;

		progress_passed = 1;
		blocknr = btrfs_node_blockptr(parent, i);
		if (last_block == 0)
			last_block = blocknr;

		if (i > 0) {
			other = btrfs_node_blockptr(parent, i - 1);
			close = close_blocks(blocknr, other, blocksize);
		}
		if (!close && i < end_slot) {
			other = btrfs_node_blockptr(parent, i + 1);
			close = close_blocks(blocknr, other, blocksize);
		}
		if (close) {
			last_block = blocknr;
			continue;
		}

		cur = btrfs_read_node_slot(parent, i);
		if (IS_ERR(cur))
			return PTR_ERR(cur);
		if (search_start == 0)
			search_start = last_block;

		btrfs_tree_lock(cur);
		err = __btrfs_cow_block(trans, root, cur, parent, i,
					&cur, search_start,
					min(16 * blocksize,
					    (end_slot - i) * blocksize),
					BTRFS_NESTING_COW);
		if (err) {
			btrfs_tree_unlock(cur);
			free_extent_buffer(cur);
			break;
		}
		search_start = cur->start;
		last_block = cur->start;
		*last_ret = search_start;
		btrfs_tree_unlock(cur);
		free_extent_buffer(cur);
	}
	return err;
}