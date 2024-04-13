SYSCALL_DEFINE2(ftruncate64, unsigned int, fd, loff_t, length)
{
	return do_sys_ftruncate(fd, length, 0);
}