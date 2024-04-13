static long btrfs_ioctl_get_dev_stats(struct btrfs_fs_info *fs_info,
				      void __user *arg)
{
	struct btrfs_ioctl_get_dev_stats *sa;
	int ret;

	sa = memdup_user(arg, sizeof(*sa));
	if (IS_ERR(sa))
		return PTR_ERR(sa);

	if ((sa->flags & BTRFS_DEV_STATS_RESET) && !capable(CAP_SYS_ADMIN)) {
		kfree(sa);
		return -EPERM;
	}

	ret = btrfs_get_dev_stats(fs_info, sa);

	if (ret == 0 && copy_to_user(arg, sa, sizeof(*sa)))
		ret = -EFAULT;

	kfree(sa);
	return ret;
}