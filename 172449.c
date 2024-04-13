static int btrfs_ioctl_ino_lookup_user(struct file *file, void __user *argp)
{
	struct btrfs_ioctl_ino_lookup_user_args *args;
	struct inode *inode;
	int ret;

	args = memdup_user(argp, sizeof(*args));
	if (IS_ERR(args))
		return PTR_ERR(args);

	inode = file_inode(file);

	if (args->dirid == BTRFS_FIRST_FREE_OBJECTID &&
	    BTRFS_I(inode)->location.objectid != BTRFS_FIRST_FREE_OBJECTID) {
		/*
		 * The subvolume does not exist under fd with which this is
		 * called
		 */
		kfree(args);
		return -EACCES;
	}

	ret = btrfs_search_path_in_tree_user(inode, args);

	if (ret == 0 && copy_to_user(argp, args, sizeof(*args)))
		ret = -EFAULT;

	kfree(args);
	return ret;
}