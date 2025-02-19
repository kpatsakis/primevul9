static int create_snapshot(struct btrfs_root *root, struct inode *dir,
			   struct dentry *dentry,
			   u64 *async_transid, bool readonly,
			   struct btrfs_qgroup_inherit *inherit)
{
	struct btrfs_fs_info *fs_info = btrfs_sb(dir->i_sb);
	struct inode *inode;
	struct btrfs_pending_snapshot *pending_snapshot;
	struct btrfs_trans_handle *trans;
	int ret;
	bool snapshot_force_cow = false;

	if (!test_bit(BTRFS_ROOT_REF_COWS, &root->state))
		return -EINVAL;

	if (atomic_read(&root->nr_swapfiles)) {
		btrfs_warn(fs_info,
			   "cannot snapshot subvolume with active swapfile");
		return -ETXTBSY;
	}

	pending_snapshot = kzalloc(sizeof(*pending_snapshot), GFP_KERNEL);
	if (!pending_snapshot)
		return -ENOMEM;

	pending_snapshot->root_item = kzalloc(sizeof(struct btrfs_root_item),
			GFP_KERNEL);
	pending_snapshot->path = btrfs_alloc_path();
	if (!pending_snapshot->root_item || !pending_snapshot->path) {
		ret = -ENOMEM;
		goto free_pending;
	}

	/*
	 * Force new buffered writes to reserve space even when NOCOW is
	 * possible. This is to avoid later writeback (running dealloc) to
	 * fallback to COW mode and unexpectedly fail with ENOSPC.
	 */
	atomic_inc(&root->will_be_snapshotted);
	smp_mb__after_atomic();
	/* wait for no snapshot writes */
	wait_event(root->subv_writers->wait,
		   percpu_counter_sum(&root->subv_writers->counter) == 0);

	ret = btrfs_start_delalloc_snapshot(root);
	if (ret)
		goto dec_and_free;

	/*
	 * All previous writes have started writeback in NOCOW mode, so now
	 * we force future writes to fallback to COW mode during snapshot
	 * creation.
	 */
	atomic_inc(&root->snapshot_force_cow);
	snapshot_force_cow = true;

	btrfs_wait_ordered_extents(root, U64_MAX, 0, (u64)-1);

	btrfs_init_block_rsv(&pending_snapshot->block_rsv,
			     BTRFS_BLOCK_RSV_TEMP);
	/*
	 * 1 - parent dir inode
	 * 2 - dir entries
	 * 1 - root item
	 * 2 - root ref/backref
	 * 1 - root of snapshot
	 * 1 - UUID item
	 */
	ret = btrfs_subvolume_reserve_metadata(BTRFS_I(dir)->root,
					&pending_snapshot->block_rsv, 8,
					false);
	if (ret)
		goto dec_and_free;

	pending_snapshot->dentry = dentry;
	pending_snapshot->root = root;
	pending_snapshot->readonly = readonly;
	pending_snapshot->dir = dir;
	pending_snapshot->inherit = inherit;

	trans = btrfs_start_transaction(root, 0);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		goto fail;
	}

	spin_lock(&fs_info->trans_lock);
	list_add(&pending_snapshot->list,
		 &trans->transaction->pending_snapshots);
	spin_unlock(&fs_info->trans_lock);
	if (async_transid) {
		*async_transid = trans->transid;
		ret = btrfs_commit_transaction_async(trans, 1);
		if (ret)
			ret = btrfs_commit_transaction(trans);
	} else {
		ret = btrfs_commit_transaction(trans);
	}
	if (ret)
		goto fail;

	ret = pending_snapshot->error;
	if (ret)
		goto fail;

	ret = btrfs_orphan_cleanup(pending_snapshot->snap);
	if (ret)
		goto fail;

	inode = btrfs_lookup_dentry(d_inode(dentry->d_parent), dentry);
	if (IS_ERR(inode)) {
		ret = PTR_ERR(inode);
		goto fail;
	}

	d_instantiate(dentry, inode);
	ret = 0;
fail:
	btrfs_subvolume_release_metadata(fs_info, &pending_snapshot->block_rsv);
dec_and_free:
	if (snapshot_force_cow)
		atomic_dec(&root->snapshot_force_cow);
	if (atomic_dec_and_test(&root->will_be_snapshotted))
		wake_up_var(&root->will_be_snapshotted);
free_pending:
	kfree(pending_snapshot->root_item);
	btrfs_free_path(pending_snapshot->path);
	kfree(pending_snapshot);

	return ret;
}