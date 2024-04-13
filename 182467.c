SMB2_tdis(const unsigned int xid, struct cifs_tcon *tcon)
{
	struct smb_rqst rqst;
	struct smb2_tree_disconnect_req *req; /* response is trivial */
	int rc = 0;
	struct cifs_ses *ses = tcon->ses;
	int flags = 0;
	unsigned int total_len;
	struct kvec iov[1];
	struct kvec rsp_iov;
	int resp_buf_type;

	cifs_dbg(FYI, "Tree Disconnect\n");

	if (!ses || !(ses->server))
		return -EIO;

	if ((tcon->need_reconnect) || (tcon->ses->need_reconnect))
		return 0;

	rc = smb2_plain_req_init(SMB2_TREE_DISCONNECT, tcon, (void **) &req,
			     &total_len);
	if (rc)
		return rc;

	if (smb3_encryption_required(tcon))
		flags |= CIFS_TRANSFORM_REQ;

	flags |= CIFS_NO_RESP;

	iov[0].iov_base = (char *)req;
	iov[0].iov_len = total_len;

	memset(&rqst, 0, sizeof(struct smb_rqst));
	rqst.rq_iov = iov;
	rqst.rq_nvec = 1;

	rc = cifs_send_recv(xid, ses, &rqst, &resp_buf_type, flags, &rsp_iov);
	cifs_small_buf_release(req);
	if (rc)
		cifs_stats_fail_inc(tcon, SMB2_TREE_DISCONNECT_HE);

	return rc;
}