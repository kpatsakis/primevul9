static long btrfs_ioctl_scrub_cancel(struct btrfs_fs_info *fs_info)
{
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	return btrfs_scrub_cancel(fs_info);
}