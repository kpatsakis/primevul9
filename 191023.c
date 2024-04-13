static void construct_reply(char *inbuf, int size, size_t unread_bytes,
			    uint32_t seqnum, bool encrypted,
			    struct smb_perfcount_data *deferred_pcd)
{
	connection_struct *conn;
	struct smb_request *req;

	if (!(req = talloc(talloc_tos(), struct smb_request))) {
		smb_panic("could not allocate smb_request");
	}

	init_smb_request(req, (uint8 *)inbuf, unread_bytes, encrypted);
	req->inbuf  = (uint8_t *)talloc_move(req, &inbuf);
	req->seqnum = seqnum;

	/* we popped this message off the queue - keep original perf data */
	if (deferred_pcd)
		req->pcd = *deferred_pcd;
	else {
		SMB_PERFCOUNT_START(&req->pcd);
		SMB_PERFCOUNT_SET_OP(&req->pcd, req->cmd);
		SMB_PERFCOUNT_SET_MSGLEN_IN(&req->pcd, size);
	}

	conn = switch_message(req->cmd, req, size);

	if (req->unread_bytes) {
		/* writeX failed. drain socket. */
		if (drain_socket(smbd_server_fd(), req->unread_bytes) !=
				req->unread_bytes) {
			smb_panic("failed to drain pending bytes");
		}
		req->unread_bytes = 0;
	}

	if (req->outbuf == NULL) {
		return;
	}

	if (CVAL(req->outbuf,0) == 0) {
		show_msg((char *)req->outbuf);
	}

	if (!srv_send_smb(smbd_server_fd(),
			(char *)req->outbuf,
			true, req->seqnum+1,
			IS_CONN_ENCRYPTED(conn)||req->encrypted,
			&req->pcd)) {
		exit_server_cleanly("construct_reply: srv_send_smb failed.");
	}

	TALLOC_FREE(req);

	return;
}