SMB2_flush(const unsigned int xid, struct cifs_tcon *tcon, u64 persistent_fid,
	   u64 volatile_fid)
{
	struct smb_rqst rqst;
	struct smb2_flush_req *req;
	struct cifs_ses *ses = tcon->ses;
	struct kvec iov[1];
	struct kvec rsp_iov;
	int resp_buftype;
	int rc = 0;
	int flags = 0;
	unsigned int total_len;

	cifs_dbg(FYI, "Flush\n");

	if (!ses || !(ses->server))
		return -EIO;

	rc = smb2_plain_req_init(SMB2_FLUSH, tcon, (void **) &req, &total_len);
	if (rc)
		return rc;

	if (smb3_encryption_required(tcon))
		flags |= CIFS_TRANSFORM_REQ;

	req->PersistentFileId = persistent_fid;
	req->VolatileFileId = volatile_fid;

	iov[0].iov_base = (char *)req;
	iov[0].iov_len = total_len;

	memset(&rqst, 0, sizeof(struct smb_rqst));
	rqst.rq_iov = iov;
	rqst.rq_nvec = 1;

	rc = cifs_send_recv(xid, ses, &rqst, &resp_buftype, flags, &rsp_iov);
	cifs_small_buf_release(req);

	if (rc != 0) {
		cifs_stats_fail_inc(tcon, SMB2_FLUSH_HE);
		trace_smb3_flush_err(xid, persistent_fid, tcon->tid, ses->Suid,
				     rc);
	}

	free_rsp_buf(resp_buftype, rsp_iov.iov_base);
	return rc;
}