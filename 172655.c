static long btrfs_ioctl_quota_ctl(struct file *file, void __user *arg)
{
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct btrfs_ioctl_quota_ctl_args *sa;
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	ret = mnt_want_write_file(file);
	if (ret)
		return ret;

	sa = memdup_user(arg, sizeof(*sa));
	if (IS_ERR(sa)) {
		ret = PTR_ERR(sa);
		goto drop_write;
	}

	down_write(&fs_info->subvol_sem);

	switch (sa->cmd) {
	case BTRFS_QUOTA_CTL_ENABLE:
		ret = btrfs_quota_enable(fs_info);
		break;
	case BTRFS_QUOTA_CTL_DISABLE:
		ret = btrfs_quota_disable(fs_info);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	kfree(sa);
	up_write(&fs_info->subvol_sem);
drop_write:
	mnt_drop_write_file(file);
	return ret;
}