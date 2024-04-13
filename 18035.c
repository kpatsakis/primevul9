static void schedule_async_open(struct timeval request_time,
				struct smb_request *req)
{
	struct deferred_open_record state;
	struct timeval timeout;

	timeout = timeval_set(20, 0);

	ZERO_STRUCT(state);
	state.delayed_for_oplocks = false;
	state.async_open = true;

	if (!request_timed_out(request_time, timeout)) {
		defer_open(NULL, request_time, timeout, req, &state);
	}
}