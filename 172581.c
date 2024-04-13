static noinline int btrfs_ioctl_tree_search_v2(struct file *file,
					       void __user *argp)
{
	struct btrfs_ioctl_search_args_v2 __user *uarg;
	struct btrfs_ioctl_search_args_v2 args;
	struct inode *inode;
	int ret;
	size_t buf_size;
	const size_t buf_limit = SZ_16M;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	/* copy search header and buffer size */
	uarg = (struct btrfs_ioctl_search_args_v2 __user *)argp;
	if (copy_from_user(&args, uarg, sizeof(args)))
		return -EFAULT;

	buf_size = args.buf_size;

	/* limit result size to 16MB */
	if (buf_size > buf_limit)
		buf_size = buf_limit;

	inode = file_inode(file);
	ret = search_ioctl(inode, &args.key, &buf_size,
			   (char __user *)(&uarg->buf[0]));
	if (ret == 0 && copy_to_user(&uarg->key, &args.key, sizeof(args.key)))
		ret = -EFAULT;
	else if (ret == -EOVERFLOW &&
		copy_to_user(&uarg->buf_size, &buf_size, sizeof(buf_size)))
		ret = -EFAULT;

	return ret;
}