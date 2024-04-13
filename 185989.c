ssize_t vfs_writev(struct file *file, const struct iovec __user *vec,
		   unsigned long vlen, loff_t *pos)
{
	if (!(file->f_mode & FMODE_WRITE))
		return -EBADF;
	if (!(file->f_mode & FMODE_CAN_WRITE))
		return -EINVAL;

	return do_readv_writev(WRITE, file, vec, vlen, pos);
}