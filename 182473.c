SMB311_posix_qfs_info(const unsigned int xid, struct cifs_tcon *tcon,
	      u64 persistent_fid, u64 volatile_fid, struct kstatfs *fsdata)
{
	struct smb_rqst rqst;
	struct smb2_query_info_rsp *rsp = NULL;
	struct kvec iov;
	struct kvec rsp_iov;
	int rc = 0;
	int resp_buftype;
	struct cifs_ses *ses = tcon->ses;
	FILE_SYSTEM_POSIX_INFO *info = NULL;
	int flags = 0;

	rc = build_qfs_info_req(&iov, tcon, FS_POSIX_INFORMATION,
				sizeof(FILE_SYSTEM_POSIX_INFO),
				persistent_fid, volatile_fid);
	if (rc)
		return rc;

	if (smb3_encryption_required(tcon))
		flags |= CIFS_TRANSFORM_REQ;

	memset(&rqst, 0, sizeof(struct smb_rqst));
	rqst.rq_iov = &iov;
	rqst.rq_nvec = 1;

	rc = cifs_send_recv(xid, ses, &rqst, &resp_buftype, flags, &rsp_iov);
	cifs_small_buf_release(iov.iov_base);
	if (rc) {
		cifs_stats_fail_inc(tcon, SMB2_QUERY_INFO_HE);
		goto posix_qfsinf_exit;
	}
	rsp = (struct smb2_query_info_rsp *)rsp_iov.iov_base;

	info = (FILE_SYSTEM_POSIX_INFO *)(
		le16_to_cpu(rsp->OutputBufferOffset) + (char *)rsp);
	rc = smb2_validate_iov(le16_to_cpu(rsp->OutputBufferOffset),
			       le32_to_cpu(rsp->OutputBufferLength), &rsp_iov,
			       sizeof(FILE_SYSTEM_POSIX_INFO));
	if (!rc)
		copy_posix_fs_info_to_kstatfs(info, fsdata);

posix_qfsinf_exit:
	free_rsp_buf(resp_buftype, rsp_iov.iov_base);
	return rc;
}