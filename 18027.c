static void defer_open(struct share_mode_lock *lck,
		       struct timeval request_time,
		       struct timeval timeout,
		       struct smb_request *req,
		       struct deferred_open_record *state)
{
	DEBUG(10,("defer_open_sharing_error: time [%u.%06u] adding deferred "
		  "open entry for mid %llu\n",
		  (unsigned int)request_time.tv_sec,
		  (unsigned int)request_time.tv_usec,
		  (unsigned long long)req->mid));

	if (!push_deferred_open_message_smb(req, request_time, timeout,
				       state->id, (char *)state, sizeof(*state))) {
		TALLOC_FREE(lck);
		exit_server("push_deferred_open_message_smb failed");
	}
	if (lck) {
		struct defer_open_state *watch_state;
		struct tevent_req *watch_req;
		bool ret;

		watch_state = talloc(req->sconn, struct defer_open_state);
		if (watch_state == NULL) {
			exit_server("talloc failed");
		}
		watch_state->sconn = req->sconn;
		watch_state->mid = req->mid;

		DEBUG(10, ("defering mid %llu\n",
			   (unsigned long long)req->mid));

		watch_req = dbwrap_record_watch_send(
			watch_state, req->sconn->ev_ctx, lck->data->record,
			req->sconn->msg_ctx);
		if (watch_req == NULL) {
			exit_server("Could not watch share mode record");
		}
		tevent_req_set_callback(watch_req, defer_open_done,
					watch_state);

		ret = tevent_req_set_endtime(
			watch_req, req->sconn->ev_ctx,
			timeval_sum(&request_time, &timeout));
		SMB_ASSERT(ret);
	}
}