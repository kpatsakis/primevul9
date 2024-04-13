signal_handler (int signo)
{
	if (signo == SIGINT || signo == SIGTERM)
		g_main_loop_quit (gl.loop);
}