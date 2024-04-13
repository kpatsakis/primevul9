static size_t compat_readv(struct file *file,
			   const struct compat_iovec __user *vec,
			   unsigned long vlen, loff_t *pos)
{
	ssize_t ret = -EBADF;

	if (!(file->f_mode & FMODE_READ))
		goto out;

	ret = -EINVAL;
	if (!(file->f_mode & FMODE_CAN_READ))
		goto out;

	ret = compat_do_readv_writev(READ, file, vec, vlen, pos);

out:
	if (ret > 0)
		add_rchar(current, ret);
	inc_syscr(current);
	return ret;
}