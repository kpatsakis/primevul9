static bool push_queued_message(struct smb_request *req,
				struct timeval request_time,
				struct timeval end_time,
				char *private_data, size_t private_len)
{
	int msg_len = smb_len(req->inbuf) + 4;
	struct pending_message_list *msg;

	msg = TALLOC_ZERO_P(NULL, struct pending_message_list);

	if(msg == NULL) {
		DEBUG(0,("push_message: malloc fail (1)\n"));
		return False;
	}

	msg->buf = data_blob_talloc(msg, req->inbuf, msg_len);
	if(msg->buf.data == NULL) {
		DEBUG(0,("push_message: malloc fail (2)\n"));
		TALLOC_FREE(msg);
		return False;
	}

	msg->request_time = request_time;
	msg->seqnum = req->seqnum;
	msg->encrypted = req->encrypted;
	SMB_PERFCOUNT_DEFER_OP(&req->pcd, &msg->pcd);

	if (private_data) {
		msg->private_data = data_blob_talloc(msg, private_data,
						     private_len);
		if (msg->private_data.data == NULL) {
			DEBUG(0,("push_message: malloc fail (3)\n"));
			TALLOC_FREE(msg);
			return False;
		}
	}

	msg->te = event_add_timed(smbd_event_context(),
				  msg,
				  end_time,
				  smbd_deferred_open_timer,
				  msg);
	if (!msg->te) {
		DEBUG(0,("push_message: event_add_timed failed\n"));
		TALLOC_FREE(msg);
		return false;
	}

	DLIST_ADD_END(deferred_open_queue, msg, struct pending_message_list *);

	DEBUG(10,("push_message: pushed message length %u on "
		  "deferred_open_queue\n", (unsigned int)msg_len));

	return True;
}