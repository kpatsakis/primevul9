int btrfs_scrub_cancel_dev(struct btrfs_fs_info *fs_info,
			   struct btrfs_device *dev)
{
	struct scrub_ctx *sctx;

	mutex_lock(&fs_info->scrub_lock);
	sctx = dev->scrub_ctx;
	if (!sctx) {
		mutex_unlock(&fs_info->scrub_lock);
		return -ENOTCONN;
	}
	atomic_inc(&sctx->cancel_req);
	while (dev->scrub_ctx) {
		mutex_unlock(&fs_info->scrub_lock);
		wait_event(fs_info->scrub_pause_wait,
			   dev->scrub_ctx == NULL);
		mutex_lock(&fs_info->scrub_lock);
	}
	mutex_unlock(&fs_info->scrub_lock);

	return 0;
}