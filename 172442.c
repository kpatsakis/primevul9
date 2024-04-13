static int _btrfs_ioctl_send(struct file *file, void __user *argp, bool compat)
{
	struct btrfs_ioctl_send_args *arg;
	int ret;

	if (compat) {
#if defined(CONFIG_64BIT) && defined(CONFIG_COMPAT)
		struct btrfs_ioctl_send_args_32 args32;

		ret = copy_from_user(&args32, argp, sizeof(args32));
		if (ret)
			return -EFAULT;
		arg = kzalloc(sizeof(*arg), GFP_KERNEL);
		if (!arg)
			return -ENOMEM;
		arg->send_fd = args32.send_fd;
		arg->clone_sources_count = args32.clone_sources_count;
		arg->clone_sources = compat_ptr(args32.clone_sources);
		arg->parent_root = args32.parent_root;
		arg->flags = args32.flags;
		memcpy(arg->reserved, args32.reserved,
		       sizeof(args32.reserved));
#else
		return -ENOTTY;
#endif
	} else {
		arg = memdup_user(argp, sizeof(*arg));
		if (IS_ERR(arg))
			return PTR_ERR(arg);
	}
	ret = btrfs_ioctl_send(file, arg);
	kfree(arg);
	return ret;
}