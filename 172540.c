int btrfs_create_uuid_tree(struct btrfs_fs_info *fs_info)
{
	struct btrfs_trans_handle *trans;
	struct btrfs_root *tree_root = fs_info->tree_root;
	struct btrfs_root *uuid_root;
	struct task_struct *task;
	int ret;

	/*
	 * 1 - root node
	 * 1 - root item
	 */
	trans = btrfs_start_transaction(tree_root, 2);
	if (IS_ERR(trans))
		return PTR_ERR(trans);

	uuid_root = btrfs_create_tree(trans, fs_info,
				      BTRFS_UUID_TREE_OBJECTID);
	if (IS_ERR(uuid_root)) {
		ret = PTR_ERR(uuid_root);
		btrfs_abort_transaction(trans, ret);
		btrfs_end_transaction(trans);
		return ret;
	}

	fs_info->uuid_root = uuid_root;

	ret = btrfs_commit_transaction(trans);
	if (ret)
		return ret;

	down(&fs_info->uuid_tree_rescan_sem);
	task = kthread_run(btrfs_uuid_scan_kthread, fs_info, "btrfs-uuid");
	if (IS_ERR(task)) {
		/* fs_info->update_uuid_tree_gen remains 0 in all error case */
		btrfs_warn(fs_info, "failed to start uuid_scan task");
		up(&fs_info->uuid_tree_rescan_sem);
		return PTR_ERR(task);
	}

	return 0;
}