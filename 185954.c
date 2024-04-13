COMPAT_SYSCALL_DEFINE3(lseek, unsigned int, fd, compat_off_t, offset, unsigned int, whence)
{
	return sys_lseek(fd, offset, whence);
}