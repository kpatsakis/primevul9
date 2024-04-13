SYSCALL_DEFINE2(ftruncate, unsigned int, fd, unsigned long, length)
{
	return do_sys_ftruncate(fd, length, 1);
}