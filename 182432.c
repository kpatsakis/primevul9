SMB2_ioctl(const unsigned int xid, struct cifs_tcon *tcon, u64 persistent_fid,
	   u64 volatile_fid, u32 opcode, bool is_fsctl,
	   char *in_data, u32 indatalen, u32 max_out_data_len,
	   char **out_data, u32 *plen /* returned data len */)
{
	struct smb_rqst rqst;
	struct smb2_ioctl_rsp *rsp = NULL;
	struct cifs_ses *ses;
	struct kvec iov[SMB2_IOCTL_IOV_SIZE];
	struct kvec rsp_iov = {NULL, 0};
	int resp_buftype = CIFS_NO_BUFFER;
	int rc = 0;
	int flags = 0;

	cifs_dbg(FYI, "SMB2 IOCTL\n");

	if (out_data != NULL)
		*out_data = NULL;

	/* zero out returned data len, in case of error */
	if (plen)
		*plen = 0;

	if (tcon)
		ses = tcon->ses;
	else
		return -EIO;

	if (!ses || !(ses->server))
		return -EIO;

	if (smb3_encryption_required(tcon))
		flags |= CIFS_TRANSFORM_REQ;

	memset(&rqst, 0, sizeof(struct smb_rqst));
	memset(&iov, 0, sizeof(iov));
	rqst.rq_iov = iov;
	rqst.rq_nvec = SMB2_IOCTL_IOV_SIZE;

	rc = SMB2_ioctl_init(tcon, &rqst, persistent_fid, volatile_fid, opcode,
			     is_fsctl, in_data, indatalen, max_out_data_len);
	if (rc)
		goto ioctl_exit;

	rc = cifs_send_recv(xid, ses, &rqst, &resp_buftype, flags,
			    &rsp_iov);
	rsp = (struct smb2_ioctl_rsp *)rsp_iov.iov_base;

	if (rc != 0)
		trace_smb3_fsctl_err(xid, persistent_fid, tcon->tid,
				ses->Suid, 0, opcode, rc);

	if ((rc != 0) && (rc != -EINVAL)) {
		cifs_stats_fail_inc(tcon, SMB2_IOCTL_HE);
		goto ioctl_exit;
	} else if (rc == -EINVAL) {
		if ((opcode != FSCTL_SRV_COPYCHUNK_WRITE) &&
		    (opcode != FSCTL_SRV_COPYCHUNK)) {
			cifs_stats_fail_inc(tcon, SMB2_IOCTL_HE);
			goto ioctl_exit;
		}
	}

	/* check if caller wants to look at return data or just return rc */
	if ((plen == NULL) || (out_data == NULL))
		goto ioctl_exit;

	*plen = le32_to_cpu(rsp->OutputCount);

	/* We check for obvious errors in the output buffer length and offset */
	if (*plen == 0)
		goto ioctl_exit; /* server returned no data */
	else if (*plen > rsp_iov.iov_len || *plen > 0xFF00) {
		cifs_dbg(VFS, "srv returned invalid ioctl length: %d\n", *plen);
		*plen = 0;
		rc = -EIO;
		goto ioctl_exit;
	}

	if (rsp_iov.iov_len - *plen < le32_to_cpu(rsp->OutputOffset)) {
		cifs_dbg(VFS, "Malformed ioctl resp: len %d offset %d\n", *plen,
			le32_to_cpu(rsp->OutputOffset));
		*plen = 0;
		rc = -EIO;
		goto ioctl_exit;
	}

	*out_data = kmemdup((char *)rsp + le32_to_cpu(rsp->OutputOffset),
			    *plen, GFP_KERNEL);
	if (*out_data == NULL) {
		rc = -ENOMEM;
		goto ioctl_exit;
	}

ioctl_exit:
	SMB2_ioctl_free(&rqst);
	free_rsp_buf(resp_buftype, rsp);
	return rc;
}