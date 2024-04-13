static long btrfs_ioctl_quota_rescan_wait(struct file *file, void __user *arg)
{
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	return btrfs_qgroup_wait_for_completion(fs_info, true);
}