noinline int btrfs_cow_block(struct btrfs_trans_handle *trans,
		    struct btrfs_root *root, struct extent_buffer *buf,
		    struct extent_buffer *parent, int parent_slot,
		    struct extent_buffer **cow_ret,
		    enum btrfs_lock_nesting nest)
{
	struct btrfs_fs_info *fs_info = root->fs_info;
	u64 search_start;
	int ret;

	if (test_bit(BTRFS_ROOT_DELETING, &root->state))
		btrfs_err(fs_info,
			"COW'ing blocks on a fs root that's being dropped");

	if (trans->transaction != fs_info->running_transaction)
		WARN(1, KERN_CRIT "trans %llu running %llu\n",
		       trans->transid,
		       fs_info->running_transaction->transid);

	if (trans->transid != fs_info->generation)
		WARN(1, KERN_CRIT "trans %llu running %llu\n",
		       trans->transid, fs_info->generation);

	if (!should_cow_block(trans, root, buf)) {
		trans->dirty = true;
		*cow_ret = buf;
		return 0;
	}

	search_start = buf->start & ~((u64)SZ_1G - 1);

	/*
	 * Before CoWing this block for later modification, check if it's
	 * the subtree root and do the delayed subtree trace if needed.
	 *
	 * Also We don't care about the error, as it's handled internally.
	 */
	btrfs_qgroup_trace_subtree_after_cow(trans, root, buf);
	ret = __btrfs_cow_block(trans, root, buf, parent,
				 parent_slot, cow_ret, search_start, 0, nest);

	trace_btrfs_cow_block(root, buf, *cow_ret);

	return ret;
}