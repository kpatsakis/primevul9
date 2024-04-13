SMB2_sess_auth_rawntlmssp_negotiate(struct SMB2_sess_data *sess_data)
{
	int rc;
	struct cifs_ses *ses = sess_data->ses;
	struct smb2_sess_setup_rsp *rsp = NULL;
	char *ntlmssp_blob = NULL;
	bool use_spnego = false; /* else use raw ntlmssp */
	u16 blob_length = 0;

	/*
	 * If memory allocation is successful, caller of this function
	 * frees it.
	 */
	ses->ntlmssp = kmalloc(sizeof(struct ntlmssp_auth), GFP_KERNEL);
	if (!ses->ntlmssp) {
		rc = -ENOMEM;
		goto out_err;
	}
	ses->ntlmssp->sesskey_per_smbsess = true;

	rc = SMB2_sess_alloc_buffer(sess_data);
	if (rc)
		goto out_err;

	ntlmssp_blob = kmalloc(sizeof(struct _NEGOTIATE_MESSAGE),
			       GFP_KERNEL);
	if (ntlmssp_blob == NULL) {
		rc = -ENOMEM;
		goto out;
	}

	build_ntlmssp_negotiate_blob(ntlmssp_blob, ses);
	if (use_spnego) {
		/* BB eventually need to add this */
		cifs_dbg(VFS, "spnego not supported for SMB2 yet\n");
		rc = -EOPNOTSUPP;
		goto out;
	} else {
		blob_length = sizeof(struct _NEGOTIATE_MESSAGE);
		/* with raw NTLMSSP we don't encapsulate in SPNEGO */
	}
	sess_data->iov[1].iov_base = ntlmssp_blob;
	sess_data->iov[1].iov_len = blob_length;

	rc = SMB2_sess_sendreceive(sess_data);
	rsp = (struct smb2_sess_setup_rsp *)sess_data->iov[0].iov_base;

	/* If true, rc here is expected and not an error */
	if (sess_data->buf0_type != CIFS_NO_BUFFER &&
		rsp->sync_hdr.Status == STATUS_MORE_PROCESSING_REQUIRED)
		rc = 0;

	if (rc)
		goto out;

	if (offsetof(struct smb2_sess_setup_rsp, Buffer) !=
			le16_to_cpu(rsp->SecurityBufferOffset)) {
		cifs_dbg(VFS, "Invalid security buffer offset %d\n",
			le16_to_cpu(rsp->SecurityBufferOffset));
		rc = -EIO;
		goto out;
	}
	rc = decode_ntlmssp_challenge(rsp->Buffer,
			le16_to_cpu(rsp->SecurityBufferLength), ses);
	if (rc)
		goto out;

	cifs_dbg(FYI, "rawntlmssp session setup challenge phase\n");


	ses->Suid = rsp->sync_hdr.SessionId;
	ses->session_flags = le16_to_cpu(rsp->SessionFlags);

out:
	kfree(ntlmssp_blob);
	SMB2_sess_free_buffer(sess_data);
	if (!rc) {
		sess_data->result = 0;
		sess_data->func = SMB2_sess_auth_rawntlmssp_authenticate;
		return;
	}
out_err:
	kfree(ses->ntlmssp);
	ses->ntlmssp = NULL;
	sess_data->result = rc;
	sess_data->func = NULL;
}