int get_unused_fd_flags(unsigned flags)
{
	return __get_unused_fd_flags(flags, rlimit(RLIMIT_NOFILE));
}