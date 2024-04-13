send_set_info(const unsigned int xid, struct cifs_tcon *tcon,
	       u64 persistent_fid, u64 volatile_fid, u32 pid, u8 info_class,
	       u8 info_type, u32 additional_info, unsigned int num,
		void **data, unsigned int *size)
{
	struct smb_rqst rqst;
	struct smb2_set_info_rsp *rsp = NULL;
	struct kvec *iov;
	struct kvec rsp_iov;
	int rc = 0;
	int resp_buftype;
	struct cifs_ses *ses = tcon->ses;
	int flags = 0;

	if (!ses || !(ses->server))
		return -EIO;

	if (!num)
		return -EINVAL;

	if (smb3_encryption_required(tcon))
		flags |= CIFS_TRANSFORM_REQ;

	iov = kmalloc_array(num, sizeof(struct kvec), GFP_KERNEL);
	if (!iov)
		return -ENOMEM;

	memset(&rqst, 0, sizeof(struct smb_rqst));
	rqst.rq_iov = iov;
	rqst.rq_nvec = num;

	rc = SMB2_set_info_init(tcon, &rqst, persistent_fid, volatile_fid, pid,
				info_class, info_type, additional_info,
				data, size);
	if (rc) {
		kfree(iov);
		return rc;
	}


	rc = cifs_send_recv(xid, ses, &rqst, &resp_buftype, flags,
			    &rsp_iov);
	SMB2_set_info_free(&rqst);
	rsp = (struct smb2_set_info_rsp *)rsp_iov.iov_base;

	if (rc != 0) {
		cifs_stats_fail_inc(tcon, SMB2_SET_INFO_HE);
		trace_smb3_set_info_err(xid, persistent_fid, tcon->tid,
				ses->Suid, info_class, (__u32)info_type, rc);
	}

	free_rsp_buf(resp_buftype, rsp);
	kfree(iov);
	return rc;
}