static long __compat_sys_preadv64(unsigned long fd,
				  const struct compat_iovec __user *vec,
				  unsigned long vlen, loff_t pos)
{
	struct fd f;
	ssize_t ret;

	if (pos < 0)
		return -EINVAL;
	f = fdget(fd);
	if (!f.file)
		return -EBADF;
	ret = -ESPIPE;
	if (f.file->f_mode & FMODE_PREAD)
		ret = compat_readv(f.file, vec, vlen, &pos);
	fdput(f);
	return ret;
}