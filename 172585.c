static noinline_for_stack void scrub_workers_put(struct btrfs_fs_info *fs_info)
{
	if (--fs_info->scrub_workers_refcnt == 0) {
		btrfs_destroy_workqueue(fs_info->scrub_workers);
		btrfs_destroy_workqueue(fs_info->scrub_wr_completion_workers);
		btrfs_destroy_workqueue(fs_info->scrub_parity_workers);
	}
	WARN_ON(fs_info->scrub_workers_refcnt < 0);
}