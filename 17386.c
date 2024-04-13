SYSCALL_DEFINE2(mkdir, const char __user *, pathname, umode_t, mode)
{
	return sys_mkdirat(AT_FDCWD, pathname, mode);
}