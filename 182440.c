query_info(const unsigned int xid, struct cifs_tcon *tcon,
	   u64 persistent_fid, u64 volatile_fid, u8 info_class, u8 info_type,
	   u32 additional_info, size_t output_len, size_t min_len, void **data,
		u32 *dlen)
{
	struct smb_rqst rqst;
	struct smb2_query_info_rsp *rsp = NULL;
	struct kvec iov[1];
	struct kvec rsp_iov;
	int rc = 0;
	int resp_buftype = CIFS_NO_BUFFER;
	struct cifs_ses *ses = tcon->ses;
	int flags = 0;
	bool allocated = false;

	cifs_dbg(FYI, "Query Info\n");

	if (!ses || !(ses->server))
		return -EIO;

	if (smb3_encryption_required(tcon))
		flags |= CIFS_TRANSFORM_REQ;

	memset(&rqst, 0, sizeof(struct smb_rqst));
	memset(&iov, 0, sizeof(iov));
	rqst.rq_iov = iov;
	rqst.rq_nvec = 1;

	rc = SMB2_query_info_init(tcon, &rqst, persistent_fid, volatile_fid,
				  info_class, info_type, additional_info,
				  output_len, 0, NULL);
	if (rc)
		goto qinf_exit;

	trace_smb3_query_info_enter(xid, persistent_fid, tcon->tid,
				    ses->Suid, info_class, (__u32)info_type);

	rc = cifs_send_recv(xid, ses, &rqst, &resp_buftype, flags, &rsp_iov);
	rsp = (struct smb2_query_info_rsp *)rsp_iov.iov_base;

	if (rc) {
		cifs_stats_fail_inc(tcon, SMB2_QUERY_INFO_HE);
		trace_smb3_query_info_err(xid, persistent_fid, tcon->tid,
				ses->Suid, info_class, (__u32)info_type, rc);
		goto qinf_exit;
	}

	trace_smb3_query_info_done(xid, persistent_fid, tcon->tid,
				ses->Suid, info_class, (__u32)info_type);

	if (dlen) {
		*dlen = le32_to_cpu(rsp->OutputBufferLength);
		if (!*data) {
			*data = kmalloc(*dlen, GFP_KERNEL);
			if (!*data) {
				cifs_dbg(VFS,
					"Error %d allocating memory for acl\n",
					rc);
				*dlen = 0;
				rc = -ENOMEM;
				goto qinf_exit;
			}
			allocated = true;
		}
	}

	rc = smb2_validate_and_copy_iov(le16_to_cpu(rsp->OutputBufferOffset),
					le32_to_cpu(rsp->OutputBufferLength),
					&rsp_iov, min_len, *data);
	if (rc && allocated) {
		kfree(*data);
		*data = NULL;
		*dlen = 0;
	}

qinf_exit:
	SMB2_query_info_free(&rqst);
	free_rsp_buf(resp_buftype, rsp);
	return rc;
}