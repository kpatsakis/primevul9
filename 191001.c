static void smbd_deferred_open_timer(struct event_context *ev,
				     struct timed_event *te,
				     struct timeval _tval,
				     void *private_data)
{
	struct pending_message_list *msg = talloc_get_type(private_data,
					   struct pending_message_list);
	TALLOC_CTX *mem_ctx = talloc_tos();
	uint8_t *inbuf;

	inbuf = (uint8_t *)talloc_memdup(mem_ctx, msg->buf.data,
					 msg->buf.length);
	if (inbuf == NULL) {
		exit_server("smbd_deferred_open_timer: talloc failed\n");
		return;
	}

	/* We leave this message on the queue so the open code can
	   know this is a retry. */
	DEBUG(5,("smbd_deferred_open_timer: trigger mid %u.\n",
		(unsigned int)SVAL(msg->buf.data,smb_mid)));

	process_smb(smbd_server_conn, inbuf,
		    msg->buf.length, 0,
		    msg->seqnum, msg->encrypted, &msg->pcd);
}