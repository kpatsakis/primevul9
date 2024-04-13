static noinline int btrfs_ioctl_subvol_getflags(struct file *file,
						void __user *arg)
{
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct btrfs_root *root = BTRFS_I(inode)->root;
	int ret = 0;
	u64 flags = 0;

	if (btrfs_ino(BTRFS_I(inode)) != BTRFS_FIRST_FREE_OBJECTID)
		return -EINVAL;

	down_read(&fs_info->subvol_sem);
	if (btrfs_root_readonly(root))
		flags |= BTRFS_SUBVOL_RDONLY;
	up_read(&fs_info->subvol_sem);

	if (copy_to_user(arg, &flags, sizeof(flags)))
		ret = -EFAULT;

	return ret;
}