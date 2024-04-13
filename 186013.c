static long ioctl_file_clone_range(struct file *file, void __user *argp)
{
	struct file_clone_range args;

	if (copy_from_user(&args, argp, sizeof(args)))
		return -EFAULT;
	return ioctl_file_clone(file, args.src_fd, args.src_offset,
				args.src_length, args.dest_offset);
}