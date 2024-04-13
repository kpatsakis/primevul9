SYSCALL_DEFINE3(readv, unsigned long, fd, const struct iovec __user *, vec,
		unsigned long, vlen)
{
	struct fd f = fdget_pos(fd);
	ssize_t ret = -EBADF;

	if (f.file) {
		loff_t pos = file_pos_read(f.file);
		ret = vfs_readv(f.file, vec, vlen, &pos);
		if (ret >= 0)
			file_pos_write(f.file, pos);
		fdput_pos(f);
	}

	if (ret > 0)
		add_rchar(current, ret);
	inc_syscr(current);
	return ret;
}