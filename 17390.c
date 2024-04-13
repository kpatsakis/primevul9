SYSCALL_DEFINE4(fallocate, int, fd, int, mode, loff_t, offset, loff_t, len)
{
	struct fd f = fdget(fd);
	int error = -EBADF;

	if (f.file) {
		error = do_fallocate(f.file, mode, offset, len);
		fdput(f);
	}
	return error;
}