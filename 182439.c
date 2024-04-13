SMB2_close_flags(const unsigned int xid, struct cifs_tcon *tcon,
		 u64 persistent_fid, u64 volatile_fid, int flags)
{
	struct smb_rqst rqst;
	struct smb2_close_rsp *rsp = NULL;
	struct cifs_ses *ses = tcon->ses;
	struct kvec iov[1];
	struct kvec rsp_iov;
	int resp_buftype = CIFS_NO_BUFFER;
	int rc = 0;

	cifs_dbg(FYI, "Close\n");

	if (!ses || !(ses->server))
		return -EIO;

	if (smb3_encryption_required(tcon))
		flags |= CIFS_TRANSFORM_REQ;

	memset(&rqst, 0, sizeof(struct smb_rqst));
	memset(&iov, 0, sizeof(iov));
	rqst.rq_iov = iov;
	rqst.rq_nvec = 1;

	rc = SMB2_close_init(tcon, &rqst, persistent_fid, volatile_fid);
	if (rc)
		goto close_exit;

	rc = cifs_send_recv(xid, ses, &rqst, &resp_buftype, flags, &rsp_iov);
	rsp = (struct smb2_close_rsp *)rsp_iov.iov_base;

	if (rc != 0) {
		cifs_stats_fail_inc(tcon, SMB2_CLOSE_HE);
		trace_smb3_close_err(xid, persistent_fid, tcon->tid, ses->Suid,
				     rc);
		goto close_exit;
	}

	atomic_dec(&tcon->num_remote_opens);

	/* BB FIXME - decode close response, update inode for caching */

close_exit:
	SMB2_close_free(&rqst);
	free_rsp_buf(resp_buftype, rsp);
	return rc;
}