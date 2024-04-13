static void schedule_defer_open(struct share_mode_lock *lck,
				struct timeval request_time,
				struct smb_request *req)
{
	struct deferred_open_record state;

	/* This is a relative time, added to the absolute
	   request_time value to get the absolute timeout time.
	   Note that if this is the second or greater time we enter
	   this codepath for this particular request mid then
	   request_time is left as the absolute time of the *first*
	   time this request mid was processed. This is what allows
	   the request to eventually time out. */

	struct timeval timeout;

	/* Normally the smbd we asked should respond within
	 * OPLOCK_BREAK_TIMEOUT seconds regardless of whether
	 * the client did, give twice the timeout as a safety
	 * measure here in case the other smbd is stuck
	 * somewhere else. */

	timeout = timeval_set(OPLOCK_BREAK_TIMEOUT*2, 0);

	/* Nothing actually uses state.delayed_for_oplocks
	   but it's handy to differentiate in debug messages
	   between a 30 second delay due to oplock break, and
	   a 1 second delay for share mode conflicts. */

	state.delayed_for_oplocks = True;
	state.async_open = false;
	state.id = lck->data->id;

	if (!request_timed_out(request_time, timeout)) {
		defer_open(lck, request_time, timeout, req, &state);
	}
}