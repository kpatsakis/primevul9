static void defer_open_done(struct tevent_req *req)
{
	struct defer_open_state *state = tevent_req_callback_data(
		req, struct defer_open_state);
	NTSTATUS status;
	bool ret;

	status = dbwrap_record_watch_recv(req, talloc_tos(), NULL);
	TALLOC_FREE(req);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(5, ("dbwrap_record_watch_recv returned %s\n",
			  nt_errstr(status)));
		/*
		 * Even if it failed, retry anyway. TODO: We need a way to
		 * tell a re-scheduled open about that error.
		 */
	}

	DEBUG(10, ("scheduling mid %llu\n", (unsigned long long)state->mid));

	ret = schedule_deferred_open_message_smb(state->sconn, state->mid);
	SMB_ASSERT(ret);
	TALLOC_FREE(state);
}