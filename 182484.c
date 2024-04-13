int smb311_posix_mkdir(const unsigned int xid, struct inode *inode,
			       umode_t mode, struct cifs_tcon *tcon,
			       const char *full_path,
			       struct cifs_sb_info *cifs_sb)
{
	struct smb_rqst rqst;
	struct smb2_create_req *req;
	struct smb2_create_rsp *rsp = NULL;
	struct cifs_ses *ses = tcon->ses;
	struct kvec iov[3]; /* make sure at least one for each open context */
	struct kvec rsp_iov = {NULL, 0};
	int resp_buftype;
	int uni_path_len;
	__le16 *copy_path = NULL;
	int copy_size;
	int rc = 0;
	unsigned int n_iov = 2;
	__u32 file_attributes = 0;
	char *pc_buf = NULL;
	int flags = 0;
	unsigned int total_len;
	__le16 *utf16_path = NULL;

	cifs_dbg(FYI, "mkdir\n");

	/* resource #1: path allocation */
	utf16_path = cifs_convert_path_to_utf16(full_path, cifs_sb);
	if (!utf16_path)
		return -ENOMEM;

	if (!ses || !(ses->server)) {
		rc = -EIO;
		goto err_free_path;
	}

	/* resource #2: request */
	rc = smb2_plain_req_init(SMB2_CREATE, tcon, (void **) &req, &total_len);
	if (rc)
		goto err_free_path;


	if (smb3_encryption_required(tcon))
		flags |= CIFS_TRANSFORM_REQ;

	req->ImpersonationLevel = IL_IMPERSONATION;
	req->DesiredAccess = cpu_to_le32(FILE_WRITE_ATTRIBUTES);
	/* File attributes ignored on open (used in create though) */
	req->FileAttributes = cpu_to_le32(file_attributes);
	req->ShareAccess = FILE_SHARE_ALL_LE;
	req->CreateDisposition = cpu_to_le32(FILE_CREATE);
	req->CreateOptions = cpu_to_le32(CREATE_NOT_FILE);

	iov[0].iov_base = (char *)req;
	/* -1 since last byte is buf[0] which is sent below (path) */
	iov[0].iov_len = total_len - 1;

	req->NameOffset = cpu_to_le16(sizeof(struct smb2_create_req));

	/* [MS-SMB2] 2.2.13 NameOffset:
	 * If SMB2_FLAGS_DFS_OPERATIONS is set in the Flags field of
	 * the SMB2 header, the file name includes a prefix that will
	 * be processed during DFS name normalization as specified in
	 * section 3.3.5.9. Otherwise, the file name is relative to
	 * the share that is identified by the TreeId in the SMB2
	 * header.
	 */
	if (tcon->share_flags & SHI1005_FLAGS_DFS) {
		int name_len;

		req->sync_hdr.Flags |= SMB2_FLAGS_DFS_OPERATIONS;
		rc = alloc_path_with_tree_prefix(&copy_path, &copy_size,
						 &name_len,
						 tcon->treeName, utf16_path);
		if (rc)
			goto err_free_req;

		req->NameLength = cpu_to_le16(name_len * 2);
		uni_path_len = copy_size;
		/* free before overwriting resource */
		kfree(utf16_path);
		utf16_path = copy_path;
	} else {
		uni_path_len = (2 * UniStrnlen((wchar_t *)utf16_path, PATH_MAX)) + 2;
		/* MUST set path len (NameLength) to 0 opening root of share */
		req->NameLength = cpu_to_le16(uni_path_len - 2);
		if (uni_path_len % 8 != 0) {
			copy_size = roundup(uni_path_len, 8);
			copy_path = kzalloc(copy_size, GFP_KERNEL);
			if (!copy_path) {
				rc = -ENOMEM;
				goto err_free_req;
			}
			memcpy((char *)copy_path, (const char *)utf16_path,
			       uni_path_len);
			uni_path_len = copy_size;
			/* free before overwriting resource */
			kfree(utf16_path);
			utf16_path = copy_path;
		}
	}

	iov[1].iov_len = uni_path_len;
	iov[1].iov_base = utf16_path;
	req->RequestedOplockLevel = SMB2_OPLOCK_LEVEL_NONE;

	if (tcon->posix_extensions) {
		/* resource #3: posix buf */
		rc = add_posix_context(iov, &n_iov, mode);
		if (rc)
			goto err_free_req;
		pc_buf = iov[n_iov-1].iov_base;
	}


	memset(&rqst, 0, sizeof(struct smb_rqst));
	rqst.rq_iov = iov;
	rqst.rq_nvec = n_iov;

	trace_smb3_posix_mkdir_enter(xid, tcon->tid, ses->Suid, CREATE_NOT_FILE,
				    FILE_WRITE_ATTRIBUTES);
	/* resource #4: response buffer */
	rc = cifs_send_recv(xid, ses, &rqst, &resp_buftype, flags, &rsp_iov);
	if (rc) {
		cifs_stats_fail_inc(tcon, SMB2_CREATE_HE);
		trace_smb3_posix_mkdir_err(xid, tcon->tid, ses->Suid,
					   CREATE_NOT_FILE,
					   FILE_WRITE_ATTRIBUTES, rc);
		goto err_free_rsp_buf;
	}

	rsp = (struct smb2_create_rsp *)rsp_iov.iov_base;
	trace_smb3_posix_mkdir_done(xid, rsp->PersistentFileId, tcon->tid,
				    ses->Suid, CREATE_NOT_FILE,
				    FILE_WRITE_ATTRIBUTES);

	SMB2_close(xid, tcon, rsp->PersistentFileId, rsp->VolatileFileId);

	/* Eventually save off posix specific response info and timestaps */

err_free_rsp_buf:
	free_rsp_buf(resp_buftype, rsp);
	kfree(pc_buf);
err_free_req:
	cifs_small_buf_release(req);
err_free_path:
	kfree(utf16_path);
	return rc;
}