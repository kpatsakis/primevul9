SYSCALL_DEFINE2(access, const char __user *, filename, int, mode)
{
	return sys_faccessat(AT_FDCWD, filename, mode);
}