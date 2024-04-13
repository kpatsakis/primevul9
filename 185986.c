static size_t compat_writev(struct file *file,
			    const struct compat_iovec __user *vec,
			    unsigned long vlen, loff_t *pos)
{
	ssize_t ret = -EBADF;

	if (!(file->f_mode & FMODE_WRITE))
		goto out;

	ret = -EINVAL;
	if (!(file->f_mode & FMODE_CAN_WRITE))
		goto out;

	ret = compat_do_readv_writev(WRITE, file, vec, vlen, pos);

out:
	if (ret > 0)
		add_wchar(current, ret);
	inc_syscw(current);
	return ret;
}