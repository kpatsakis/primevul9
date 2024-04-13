static noinline_for_stack int scrub_workers_get(struct btrfs_fs_info *fs_info,
						int is_dev_replace)
{
	unsigned int flags = WQ_FREEZABLE | WQ_UNBOUND;
	int max_active = fs_info->thread_pool_size;

	if (fs_info->scrub_workers_refcnt == 0) {
		fs_info->scrub_workers = btrfs_alloc_workqueue(fs_info, "scrub",
				flags, is_dev_replace ? 1 : max_active, 4);
		if (!fs_info->scrub_workers)
			goto fail_scrub_workers;

		fs_info->scrub_wr_completion_workers =
			btrfs_alloc_workqueue(fs_info, "scrubwrc", flags,
					      max_active, 2);
		if (!fs_info->scrub_wr_completion_workers)
			goto fail_scrub_wr_completion_workers;

		fs_info->scrub_parity_workers =
			btrfs_alloc_workqueue(fs_info, "scrubparity", flags,
					      max_active, 2);
		if (!fs_info->scrub_parity_workers)
			goto fail_scrub_parity_workers;
	}
	++fs_info->scrub_workers_refcnt;
	return 0;

fail_scrub_parity_workers:
	btrfs_destroy_workqueue(fs_info->scrub_wr_completion_workers);
fail_scrub_wr_completion_workers:
	btrfs_destroy_workqueue(fs_info->scrub_workers);
fail_scrub_workers:
	return -ENOMEM;
}