SYSCALL_DEFINE2(creat, const char __user *, pathname, umode_t, mode)
{
	return sys_open(pathname, O_CREAT | O_WRONLY | O_TRUNC, mode);
}