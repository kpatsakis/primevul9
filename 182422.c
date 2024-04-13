SMB2_sess_auth_rawntlmssp_authenticate(struct SMB2_sess_data *sess_data)
{
	int rc;
	struct cifs_ses *ses = sess_data->ses;
	struct smb2_sess_setup_req *req;
	struct smb2_sess_setup_rsp *rsp = NULL;
	unsigned char *ntlmssp_blob = NULL;
	bool use_spnego = false; /* else use raw ntlmssp */
	u16 blob_length = 0;

	rc = SMB2_sess_alloc_buffer(sess_data);
	if (rc)
		goto out;

	req = (struct smb2_sess_setup_req *) sess_data->iov[0].iov_base;
	req->sync_hdr.SessionId = ses->Suid;

	rc = build_ntlmssp_auth_blob(&ntlmssp_blob, &blob_length, ses,
					sess_data->nls_cp);
	if (rc) {
		cifs_dbg(FYI, "build_ntlmssp_auth_blob failed %d\n", rc);
		goto out;
	}

	if (use_spnego) {
		/* BB eventually need to add this */
		cifs_dbg(VFS, "spnego not supported for SMB2 yet\n");
		rc = -EOPNOTSUPP;
		goto out;
	}
	sess_data->iov[1].iov_base = ntlmssp_blob;
	sess_data->iov[1].iov_len = blob_length;

	rc = SMB2_sess_sendreceive(sess_data);
	if (rc)
		goto out;

	rsp = (struct smb2_sess_setup_rsp *)sess_data->iov[0].iov_base;

	ses->Suid = rsp->sync_hdr.SessionId;
	ses->session_flags = le16_to_cpu(rsp->SessionFlags);

	rc = SMB2_sess_establish_session(sess_data);
out:
	kfree(ntlmssp_blob);
	SMB2_sess_free_buffer(sess_data);
	kfree(ses->ntlmssp);
	ses->ntlmssp = NULL;
	sess_data->result = rc;
	sess_data->func = NULL;
}