SYSCALL_DEFINE0(inotify_init)
{
	return do_inotify_init(0);
}