static int btrfs_uuid_rescan_kthread(void *data)
{
	struct btrfs_fs_info *fs_info = (struct btrfs_fs_info *)data;
	int ret;

	/*
	 * 1st step is to iterate through the existing UUID tree and
	 * to delete all entries that contain outdated data.
	 * 2nd step is to add all missing entries to the UUID tree.
	 */
	ret = btrfs_uuid_tree_iterate(fs_info, btrfs_check_uuid_tree_entry);
	if (ret < 0) {
		btrfs_warn(fs_info, "iterating uuid_tree failed %d", ret);
		up(&fs_info->uuid_tree_rescan_sem);
		return ret;
	}
	return btrfs_uuid_scan_kthread(data);
}