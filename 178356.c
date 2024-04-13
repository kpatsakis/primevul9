void sig_int(int sig_num)
{
	/* KLUGE: if the user hits Ctrl-C while ssh is prompting
	 * for a password, then our cleanup's sending of a SIGUSR1
	 * signal to all our children may kill ssh before it has a
	 * chance to restore the tty settings (i.e. turn echo back
	 * on).  By sleeping for a short time, ssh gets a bigger
	 * chance to do the right thing.  If child processes are
	 * not ssh waiting for a password, then this tiny delay
	 * shouldn't hurt anything. */
	msleep(400);

	/* If we're an rsync daemon listener (not a daemon server),
	 * we'll exit with status 0 if we received SIGTERM. */
	if (am_daemon && !am_server && sig_num == SIGTERM)
		exit_cleanup(0);

	/* If the signal arrived on the server side (or for the receiver
	 * process on the client), we want to try to do a controlled shutdown
	 * that lets the client side (generator process) know what happened.
	 * To do this, we set a flag and let the normal process handle the
	 * shutdown.  We only attempt this if multiplexed IO is in effect and
	 * we didn't already set the flag. */
	if (!got_kill_signal && (am_server || am_receiver)) {
		got_kill_signal = sig_num;
		return;
	}

	exit_cleanup(RERR_SIGNAL);
}