SYSCALL_DEFINE2(link, const char __user *, oldname, const char __user *, newname)
{
	return sys_linkat(AT_FDCWD, oldname, AT_FDCWD, newname, 0);
}