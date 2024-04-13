static void smbd_sig_hup_handler(struct tevent_context *ev,
				  struct tevent_signal *se,
				  int signum,
				  int count,
				  void *siginfo,
				  void *private_data)
{
	change_to_root_user();
	DEBUG(1,("Reloading services after SIGHUP\n"));
	reload_services(False);
}