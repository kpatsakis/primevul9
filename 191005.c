static NTSTATUS smbd_server_connection_loop_once(struct smbd_server_connection *conn)
{
	fd_set r_fds, w_fds;
	int selrtn;
	struct timeval to;
	int maxfd = 0;

	to.tv_sec = SMBD_SELECT_TIMEOUT;
	to.tv_usec = 0;

	/*
	 * Setup the select fd sets.
	 */

	FD_ZERO(&r_fds);
	FD_ZERO(&w_fds);

	/*
	 * Are there any timed events waiting ? If so, ensure we don't
	 * select for longer than it would take to wait for them.
	 */

	{
		struct timeval now;
		GetTimeOfDay(&now);

		event_add_to_select_args(smbd_event_context(), &now,
					 &r_fds, &w_fds, &to, &maxfd);
	}

	/* Process a signal and timed events now... */
	if (run_events(smbd_event_context(), 0, NULL, NULL)) {
		return NT_STATUS_RETRY;
	}

	{
		int sav;
		START_PROFILE(smbd_idle);

		selrtn = sys_select(maxfd+1,&r_fds,&w_fds,NULL,&to);
		sav = errno;

		END_PROFILE(smbd_idle);
		errno = sav;
	}

	if (run_events(smbd_event_context(), selrtn, &r_fds, &w_fds)) {
		return NT_STATUS_RETRY;
	}

	/* Check if error */
	if (selrtn == -1) {
		/* something is wrong. Maybe the socket is dead? */
		return map_nt_error_from_unix(errno);
	}

	/* Did we timeout ? */
	if (selrtn == 0) {
		return NT_STATUS_RETRY;
	}

	/* should not be reached */
	return NT_STATUS_INTERNAL_ERROR;
}