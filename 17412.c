SYSCALL_DEFINE2(truncate, const char __user *, path, long, length)
{
	return do_sys_truncate(path, length);
}