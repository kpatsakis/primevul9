bool push_deferred_smb_message(struct smb_request *req,
			       struct timeval request_time,
			       struct timeval timeout,
			       char *private_data, size_t priv_len)
{
	struct timeval end_time;

	if (req->unread_bytes) {
		DEBUG(0,("push_deferred_smb_message: logic error ! "
			"unread_bytes = %u\n",
			(unsigned int)req->unread_bytes ));
		smb_panic("push_deferred_smb_message: "
			"logic error unread_bytes != 0" );
	}

	end_time = timeval_sum(&request_time, &timeout);

	DEBUG(10,("push_deferred_open_smb_message: pushing message len %u mid %u "
		  "timeout time [%u.%06u]\n",
		  (unsigned int) smb_len(req->inbuf)+4, (unsigned int)req->mid,
		  (unsigned int)end_time.tv_sec,
		  (unsigned int)end_time.tv_usec));

	return push_queued_message(req, request_time, end_time,
				   private_data, priv_len);
}