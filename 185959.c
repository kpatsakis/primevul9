static int file_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg)
{
	struct inode *inode = file_inode(filp);
	int __user *p = (int __user *)arg;

	switch (cmd) {
	case FIBMAP:
		return ioctl_fibmap(filp, p);
	case FIONREAD:
		return put_user(i_size_read(inode) - filp->f_pos, p);
	case FS_IOC_RESVSP:
	case FS_IOC_RESVSP64:
		return ioctl_preallocate(filp, p);
	}

	return vfs_ioctl(filp, cmd, arg);
}