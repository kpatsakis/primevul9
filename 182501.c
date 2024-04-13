SMB2_tcon(const unsigned int xid, struct cifs_ses *ses, const char *tree,
	  struct cifs_tcon *tcon, const struct nls_table *cp)
{
	struct smb_rqst rqst;
	struct smb2_tree_connect_req *req;
	struct smb2_tree_connect_rsp *rsp = NULL;
	struct kvec iov[2];
	struct kvec rsp_iov = { NULL, 0 };
	int rc = 0;
	int resp_buftype;
	int unc_path_len;
	__le16 *unc_path = NULL;
	int flags = 0;
	unsigned int total_len;

	cifs_dbg(FYI, "TCON\n");

	if (!(ses->server) || !tree)
		return -EIO;

	unc_path = kmalloc(MAX_SHARENAME_LENGTH * 2, GFP_KERNEL);
	if (unc_path == NULL)
		return -ENOMEM;

	unc_path_len = cifs_strtoUTF16(unc_path, tree, strlen(tree), cp) + 1;
	unc_path_len *= 2;
	if (unc_path_len < 2) {
		kfree(unc_path);
		return -EINVAL;
	}

	/* SMB2 TREE_CONNECT request must be called with TreeId == 0 */
	tcon->tid = 0;
	atomic_set(&tcon->num_remote_opens, 0);
	rc = smb2_plain_req_init(SMB2_TREE_CONNECT, tcon, (void **) &req,
			     &total_len);
	if (rc) {
		kfree(unc_path);
		return rc;
	}

	if (smb3_encryption_required(tcon))
		flags |= CIFS_TRANSFORM_REQ;

	iov[0].iov_base = (char *)req;
	/* 1 for pad */
	iov[0].iov_len = total_len - 1;

	/* Testing shows that buffer offset must be at location of Buffer[0] */
	req->PathOffset = cpu_to_le16(sizeof(struct smb2_tree_connect_req)
			- 1 /* pad */);
	req->PathLength = cpu_to_le16(unc_path_len - 2);
	iov[1].iov_base = unc_path;
	iov[1].iov_len = unc_path_len;

	/*
	 * 3.11 tcon req must be signed if not encrypted. See MS-SMB2 3.2.4.1.1
	 * unless it is guest or anonymous user. See MS-SMB2 3.2.5.3.1
	 * (Samba servers don't always set the flag so also check if null user)
	 */
	if ((ses->server->dialect == SMB311_PROT_ID) &&
	    !smb3_encryption_required(tcon) &&
	    !(ses->session_flags &
		    (SMB2_SESSION_FLAG_IS_GUEST|SMB2_SESSION_FLAG_IS_NULL)) &&
	    ((ses->user_name != NULL) || (ses->sectype == Kerberos)))
		req->sync_hdr.Flags |= SMB2_FLAGS_SIGNED;

	memset(&rqst, 0, sizeof(struct smb_rqst));
	rqst.rq_iov = iov;
	rqst.rq_nvec = 2;

	/* Need 64 for max size write so ask for more in case not there yet */
	req->sync_hdr.CreditRequest = cpu_to_le16(64);

	rc = cifs_send_recv(xid, ses, &rqst, &resp_buftype, flags, &rsp_iov);
	cifs_small_buf_release(req);
	rsp = (struct smb2_tree_connect_rsp *)rsp_iov.iov_base;
	trace_smb3_tcon(xid, tcon->tid, ses->Suid, tree, rc);
	if (rc != 0) {
		if (tcon) {
			cifs_stats_fail_inc(tcon, SMB2_TREE_CONNECT_HE);
			tcon->need_reconnect = true;
		}
		goto tcon_error_exit;
	}

	switch (rsp->ShareType) {
	case SMB2_SHARE_TYPE_DISK:
		cifs_dbg(FYI, "connection to disk share\n");
		break;
	case SMB2_SHARE_TYPE_PIPE:
		tcon->pipe = true;
		cifs_dbg(FYI, "connection to pipe share\n");
		break;
	case SMB2_SHARE_TYPE_PRINT:
		tcon->print = true;
		cifs_dbg(FYI, "connection to printer\n");
		break;
	default:
		cifs_dbg(VFS, "unknown share type %d\n", rsp->ShareType);
		rc = -EOPNOTSUPP;
		goto tcon_error_exit;
	}

	tcon->share_flags = le32_to_cpu(rsp->ShareFlags);
	tcon->capabilities = rsp->Capabilities; /* we keep caps little endian */
	tcon->maximal_access = le32_to_cpu(rsp->MaximalAccess);
	tcon->tidStatus = CifsGood;
	tcon->need_reconnect = false;
	tcon->tid = rsp->sync_hdr.TreeId;
	strlcpy(tcon->treeName, tree, sizeof(tcon->treeName));

	if ((rsp->Capabilities & SMB2_SHARE_CAP_DFS) &&
	    ((tcon->share_flags & SHI1005_FLAGS_DFS) == 0))
		cifs_dbg(VFS, "DFS capability contradicts DFS flag\n");

	if (tcon->seal &&
	    !(tcon->ses->server->capabilities & SMB2_GLOBAL_CAP_ENCRYPTION))
		cifs_dbg(VFS, "Encryption is requested but not supported\n");

	init_copy_chunk_defaults(tcon);
	if (tcon->ses->server->ops->validate_negotiate)
		rc = tcon->ses->server->ops->validate_negotiate(xid, tcon);
tcon_exit:

	free_rsp_buf(resp_buftype, rsp);
	kfree(unc_path);
	return rc;

tcon_error_exit:
	if (rsp && rsp->sync_hdr.Status == STATUS_BAD_NETWORK_NAME) {
		cifs_dbg(VFS, "BAD_NETWORK_NAME: %s\n", tree);
	}
	goto tcon_exit;
}