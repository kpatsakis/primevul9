COMPAT_SYSCALL_DEFINE2(truncate, const char __user *, path, compat_off_t, length)
{
	return do_sys_truncate(path, length);
}