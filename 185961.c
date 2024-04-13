SYSCALL_DEFINE3(lseek, unsigned int, fd, off_t, offset, unsigned int, whence)
{
	off_t retval;
	struct fd f = fdget_pos(fd);
	if (!f.file)
		return -EBADF;

	retval = -EINVAL;
	if (whence <= SEEK_MAX) {
		loff_t res = vfs_llseek(f.file, offset, whence);
		retval = res;
		if (res != (loff_t)retval)
			retval = -EOVERFLOW;	/* LFS: should only happen on 32 bit platforms */
	}
	fdput_pos(f);
	return retval;
}