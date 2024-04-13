handle_kill_daemon(context *ctx UNUSED,
		   struct pollfd *pollfd UNUSED,
		   socklen_t size UNUSED)
{
	should_exit = 1;
}