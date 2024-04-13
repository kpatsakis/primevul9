smb2_lockv(const unsigned int xid, struct cifs_tcon *tcon,
	   const __u64 persist_fid, const __u64 volatile_fid, const __u32 pid,
	   const __u32 num_lock, struct smb2_lock_element *buf)
{
	struct smb_rqst rqst;
	int rc = 0;
	struct smb2_lock_req *req = NULL;
	struct kvec iov[2];
	struct kvec rsp_iov;
	int resp_buf_type;
	unsigned int count;
	int flags = CIFS_NO_RESP;
	unsigned int total_len;

	cifs_dbg(FYI, "smb2_lockv num lock %d\n", num_lock);

	rc = smb2_plain_req_init(SMB2_LOCK, tcon, (void **) &req, &total_len);
	if (rc)
		return rc;

	if (smb3_encryption_required(tcon))
		flags |= CIFS_TRANSFORM_REQ;

	req->sync_hdr.ProcessId = cpu_to_le32(pid);
	req->LockCount = cpu_to_le16(num_lock);

	req->PersistentFileId = persist_fid;
	req->VolatileFileId = volatile_fid;

	count = num_lock * sizeof(struct smb2_lock_element);

	iov[0].iov_base = (char *)req;
	iov[0].iov_len = total_len - sizeof(struct smb2_lock_element);
	iov[1].iov_base = (char *)buf;
	iov[1].iov_len = count;

	cifs_stats_inc(&tcon->stats.cifs_stats.num_locks);

	memset(&rqst, 0, sizeof(struct smb_rqst));
	rqst.rq_iov = iov;
	rqst.rq_nvec = 2;

	rc = cifs_send_recv(xid, tcon->ses, &rqst, &resp_buf_type, flags,
			    &rsp_iov);
	cifs_small_buf_release(req);
	if (rc) {
		cifs_dbg(FYI, "Send error in smb2_lockv = %d\n", rc);
		cifs_stats_fail_inc(tcon, SMB2_LOCK_HE);
		trace_smb3_lock_err(xid, persist_fid, tcon->tid,
				    tcon->ses->Suid, rc);
	}

	return rc;
}