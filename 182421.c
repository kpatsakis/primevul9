SMB2_sess_alloc_buffer(struct SMB2_sess_data *sess_data)
{
	int rc;
	struct cifs_ses *ses = sess_data->ses;
	struct smb2_sess_setup_req *req;
	struct TCP_Server_Info *server = ses->server;
	unsigned int total_len;

	rc = smb2_plain_req_init(SMB2_SESSION_SETUP, NULL, (void **) &req,
			     &total_len);
	if (rc)
		return rc;

	/* First session, not a reauthenticate */
	req->sync_hdr.SessionId = 0;

	/* if reconnect, we need to send previous sess id, otherwise it is 0 */
	req->PreviousSessionId = sess_data->previous_session;

	req->Flags = 0; /* MBZ */

	/* enough to enable echos and oplocks and one max size write */
	req->sync_hdr.CreditRequest = cpu_to_le16(130);

	/* only one of SMB2 signing flags may be set in SMB2 request */
	if (server->sign)
		req->SecurityMode = SMB2_NEGOTIATE_SIGNING_REQUIRED;
	else if (global_secflags & CIFSSEC_MAY_SIGN) /* one flag unlike MUST_ */
		req->SecurityMode = SMB2_NEGOTIATE_SIGNING_ENABLED;
	else
		req->SecurityMode = 0;

	req->Capabilities = 0;
	req->Channel = 0; /* MBZ */

	sess_data->iov[0].iov_base = (char *)req;
	/* 1 for pad */
	sess_data->iov[0].iov_len = total_len - 1;
	/*
	 * This variable will be used to clear the buffer
	 * allocated above in case of any error in the calling function.
	 */
	sess_data->buf0_type = CIFS_SMALL_BUFFER;

	return 0;
}