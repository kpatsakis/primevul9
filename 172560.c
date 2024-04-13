static long btrfs_ioctl_quota_rescan_status(struct file *file, void __user *arg)
{
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct btrfs_ioctl_quota_rescan_args *qsa;
	int ret = 0;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	qsa = kzalloc(sizeof(*qsa), GFP_KERNEL);
	if (!qsa)
		return -ENOMEM;

	if (fs_info->qgroup_flags & BTRFS_QGROUP_STATUS_FLAG_RESCAN) {
		qsa->flags = 1;
		qsa->progress = fs_info->qgroup_rescan_progress.objectid;
	}

	if (copy_to_user(arg, qsa, sizeof(*qsa)))
		ret = -EFAULT;

	kfree(qsa);
	return ret;
}