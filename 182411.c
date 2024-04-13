SMB2_logoff(const unsigned int xid, struct cifs_ses *ses)
{
	struct smb_rqst rqst;
	struct smb2_logoff_req *req; /* response is also trivial struct */
	int rc = 0;
	struct TCP_Server_Info *server;
	int flags = 0;
	unsigned int total_len;
	struct kvec iov[1];
	struct kvec rsp_iov;
	int resp_buf_type;

	cifs_dbg(FYI, "disconnect session %p\n", ses);

	if (ses && (ses->server))
		server = ses->server;
	else
		return -EIO;

	/* no need to send SMB logoff if uid already closed due to reconnect */
	if (ses->need_reconnect)
		goto smb2_session_already_dead;

	rc = smb2_plain_req_init(SMB2_LOGOFF, NULL, (void **) &req, &total_len);
	if (rc)
		return rc;

	 /* since no tcon, smb2_init can not do this, so do here */
	req->sync_hdr.SessionId = ses->Suid;

	if (ses->session_flags & SMB2_SESSION_FLAG_ENCRYPT_DATA)
		flags |= CIFS_TRANSFORM_REQ;
	else if (server->sign)
		req->sync_hdr.Flags |= SMB2_FLAGS_SIGNED;

	flags |= CIFS_NO_RESP;

	iov[0].iov_base = (char *)req;
	iov[0].iov_len = total_len;

	memset(&rqst, 0, sizeof(struct smb_rqst));
	rqst.rq_iov = iov;
	rqst.rq_nvec = 1;

	rc = cifs_send_recv(xid, ses, &rqst, &resp_buf_type, flags, &rsp_iov);
	cifs_small_buf_release(req);
	/*
	 * No tcon so can't do
	 * cifs_stats_inc(&tcon->stats.smb2_stats.smb2_com_fail[SMB2...]);
	 */

smb2_session_already_dead:
	return rc;
}