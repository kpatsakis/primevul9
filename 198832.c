SYSCALL_DEFINE1(inotify_init1, int, flags)
{
	return do_inotify_init(flags);
}