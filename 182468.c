SMB2_oplock_break(const unsigned int xid, struct cifs_tcon *tcon,
		  const u64 persistent_fid, const u64 volatile_fid,
		  __u8 oplock_level)
{
	struct smb_rqst rqst;
	int rc;
	struct smb2_oplock_break *req = NULL;
	struct cifs_ses *ses = tcon->ses;
	int flags = CIFS_OBREAK_OP;
	unsigned int total_len;
	struct kvec iov[1];
	struct kvec rsp_iov;
	int resp_buf_type;

	cifs_dbg(FYI, "SMB2_oplock_break\n");
	rc = smb2_plain_req_init(SMB2_OPLOCK_BREAK, tcon, (void **) &req,
			     &total_len);
	if (rc)
		return rc;

	if (smb3_encryption_required(tcon))
		flags |= CIFS_TRANSFORM_REQ;

	req->VolatileFid = volatile_fid;
	req->PersistentFid = persistent_fid;
	req->OplockLevel = oplock_level;
	req->sync_hdr.CreditRequest = cpu_to_le16(1);

	flags |= CIFS_NO_RESP;

	iov[0].iov_base = (char *)req;
	iov[0].iov_len = total_len;

	memset(&rqst, 0, sizeof(struct smb_rqst));
	rqst.rq_iov = iov;
	rqst.rq_nvec = 1;

	rc = cifs_send_recv(xid, ses, &rqst, &resp_buf_type, flags, &rsp_iov);
	cifs_small_buf_release(req);

	if (rc) {
		cifs_stats_fail_inc(tcon, SMB2_OPLOCK_BREAK_HE);
		cifs_dbg(FYI, "Send error in Oplock Break = %d\n", rc);
	}

	return rc;
}