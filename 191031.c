static void smbd_sig_term_handler(struct tevent_context *ev,
				  struct tevent_signal *se,
				  int signum,
				  int count,
				  void *siginfo,
				  void *private_data)
{
	exit_server_cleanly("termination signal");
}