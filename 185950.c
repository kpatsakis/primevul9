static int w_long(unsigned int fd, unsigned int cmd,
		compat_ulong_t __user *argp)
{
	mm_segment_t old_fs = get_fs();
	int err;
	unsigned long val;

	set_fs (KERNEL_DS);
	err = sys_ioctl(fd, cmd, (unsigned long)&val);
	set_fs (old_fs);
	if (!err && put_user(val, argp))
		return -EFAULT;
	return err;
}