static long btrfs_ioctl_scrub_progress(struct btrfs_fs_info *fs_info,
				       void __user *arg)
{
	struct btrfs_ioctl_scrub_args *sa;
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	sa = memdup_user(arg, sizeof(*sa));
	if (IS_ERR(sa))
		return PTR_ERR(sa);

	ret = btrfs_scrub_progress(fs_info, sa->devid, &sa->progress);

	if (ret == 0 && copy_to_user(arg, sa, sizeof(*sa)))
		ret = -EFAULT;

	kfree(sa);
	return ret;
}