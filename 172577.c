static long btrfs_ioctl_scrub(struct file *file, void __user *arg)
{
	struct btrfs_fs_info *fs_info = btrfs_sb(file_inode(file)->i_sb);
	struct btrfs_ioctl_scrub_args *sa;
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	sa = memdup_user(arg, sizeof(*sa));
	if (IS_ERR(sa))
		return PTR_ERR(sa);

	if (!(sa->flags & BTRFS_SCRUB_READONLY)) {
		ret = mnt_want_write_file(file);
		if (ret)
			goto out;
	}

	ret = btrfs_scrub_dev(fs_info, sa->devid, sa->start, sa->end,
			      &sa->progress, sa->flags & BTRFS_SCRUB_READONLY,
			      0);

	if (ret == 0 && copy_to_user(arg, sa, sizeof(*sa)))
		ret = -EFAULT;

	if (!(sa->flags & BTRFS_SCRUB_READONLY))
		mnt_drop_write_file(file);
out:
	kfree(sa);
	return ret;
}