imapx_server_child_process_setup (gpointer user_data)
{
#ifdef TIOCNOTTY
	gint fd;
#endif

	setsid ();

#ifdef TIOCNOTTY
	/* Detach from the controlling tty if we have one.  Otherwise,
	 * SSH might do something stupid like trying to use it instead
	 * of running $SSH_ASKPASS. */
	if ((fd = open ("/dev/tty", O_RDONLY)) != -1) {
		ioctl (fd, TIOCNOTTY, NULL);
		close (fd);
	}
#endif /* TIOCNOTTY */
}