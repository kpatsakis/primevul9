static int btrfs_ioctl_fsgetxattr(struct file *file, void __user *arg)
{
	struct btrfs_inode *binode = BTRFS_I(file_inode(file));
	struct fsxattr fa;

	memset(&fa, 0, sizeof(fa));
	fa.fsx_xflags = btrfs_inode_flags_to_xflags(binode->flags);

	if (copy_to_user(arg, &fa, sizeof(fa)))
		return -EFAULT;

	return 0;
}