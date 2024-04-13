ssize_t vfs_readv(struct file *file, const struct iovec __user *vec,
		  unsigned long vlen, loff_t *pos)
{
	if (!(file->f_mode & FMODE_READ))
		return -EBADF;
	if (!(file->f_mode & FMODE_CAN_READ))
		return -EINVAL;

	return do_readv_writev(READ, file, vec, vlen, pos);
}