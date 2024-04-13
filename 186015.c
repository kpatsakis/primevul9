SYSCALL_DEFINE3(ioctl, unsigned int, fd, unsigned int, cmd, unsigned long, arg)
{
	int error;
	struct fd f = fdget(fd);

	if (!f.file)
		return -EBADF;
	error = security_file_ioctl(f.file, cmd, arg);
	if (!error)
		error = do_vfs_ioctl(f.file, fd, cmd, arg);
	fdput(f);
	return error;
}