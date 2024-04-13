SMB2_lease_break(const unsigned int xid, struct cifs_tcon *tcon,
		 __u8 *lease_key, const __le32 lease_state)
{
	struct smb_rqst rqst;
	int rc;
	struct smb2_lease_ack *req = NULL;
	struct cifs_ses *ses = tcon->ses;
	int flags = CIFS_OBREAK_OP;
	unsigned int total_len;
	struct kvec iov[1];
	struct kvec rsp_iov;
	int resp_buf_type;
	__u64 *please_key_high;
	__u64 *please_key_low;

	cifs_dbg(FYI, "SMB2_lease_break\n");
	rc = smb2_plain_req_init(SMB2_OPLOCK_BREAK, tcon, (void **) &req,
			     &total_len);
	if (rc)
		return rc;

	if (smb3_encryption_required(tcon))
		flags |= CIFS_TRANSFORM_REQ;

	req->sync_hdr.CreditRequest = cpu_to_le16(1);
	req->StructureSize = cpu_to_le16(36);
	total_len += 12;

	memcpy(req->LeaseKey, lease_key, 16);
	req->LeaseState = lease_state;

	flags |= CIFS_NO_RESP;

	iov[0].iov_base = (char *)req;
	iov[0].iov_len = total_len;

	memset(&rqst, 0, sizeof(struct smb_rqst));
	rqst.rq_iov = iov;
	rqst.rq_nvec = 1;

	rc = cifs_send_recv(xid, ses, &rqst, &resp_buf_type, flags, &rsp_iov);
	cifs_small_buf_release(req);

	please_key_low = (__u64 *)lease_key;
	please_key_high = (__u64 *)(lease_key+8);
	if (rc) {
		cifs_stats_fail_inc(tcon, SMB2_OPLOCK_BREAK_HE);
		trace_smb3_lease_err(le32_to_cpu(lease_state), tcon->tid,
			ses->Suid, *please_key_low, *please_key_high, rc);
		cifs_dbg(FYI, "Send error in Lease Break = %d\n", rc);
	} else
		trace_smb3_lease_done(le32_to_cpu(lease_state), tcon->tid,
			ses->Suid, *please_key_low, *please_key_high);

	return rc;
}