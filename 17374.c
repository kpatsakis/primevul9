SYSCALL_DEFINE1(unlink, const char __user *, pathname)
{
	return do_unlinkat(AT_FDCWD, pathname);
}