static int btrfs_ioctl_getflags(struct file *file, void __user *arg)
{
	struct btrfs_inode *binode = BTRFS_I(file_inode(file));
	unsigned int flags = btrfs_inode_flags_to_fsflags(binode->flags);

	if (copy_to_user(arg, &flags, sizeof(flags)))
		return -EFAULT;
	return 0;
}