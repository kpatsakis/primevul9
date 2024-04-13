SMB2_auth_kerberos(struct SMB2_sess_data *sess_data)
{
	int rc;
	struct cifs_ses *ses = sess_data->ses;
	struct cifs_spnego_msg *msg;
	struct key *spnego_key = NULL;
	struct smb2_sess_setup_rsp *rsp = NULL;

	rc = SMB2_sess_alloc_buffer(sess_data);
	if (rc)
		goto out;

	spnego_key = cifs_get_spnego_key(ses);
	if (IS_ERR(spnego_key)) {
		rc = PTR_ERR(spnego_key);
		spnego_key = NULL;
		goto out;
	}

	msg = spnego_key->payload.data[0];
	/*
	 * check version field to make sure that cifs.upcall is
	 * sending us a response in an expected form
	 */
	if (msg->version != CIFS_SPNEGO_UPCALL_VERSION) {
		cifs_dbg(VFS,
			  "bad cifs.upcall version. Expected %d got %d",
			  CIFS_SPNEGO_UPCALL_VERSION, msg->version);
		rc = -EKEYREJECTED;
		goto out_put_spnego_key;
	}

	ses->auth_key.response = kmemdup(msg->data, msg->sesskey_len,
					 GFP_KERNEL);
	if (!ses->auth_key.response) {
		cifs_dbg(VFS,
			"Kerberos can't allocate (%u bytes) memory",
			msg->sesskey_len);
		rc = -ENOMEM;
		goto out_put_spnego_key;
	}
	ses->auth_key.len = msg->sesskey_len;

	sess_data->iov[1].iov_base = msg->data + msg->sesskey_len;
	sess_data->iov[1].iov_len = msg->secblob_len;

	rc = SMB2_sess_sendreceive(sess_data);
	if (rc)
		goto out_put_spnego_key;

	rsp = (struct smb2_sess_setup_rsp *)sess_data->iov[0].iov_base;
	ses->Suid = rsp->sync_hdr.SessionId;

	ses->session_flags = le16_to_cpu(rsp->SessionFlags);

	rc = SMB2_sess_establish_session(sess_data);
out_put_spnego_key:
	key_invalidate(spnego_key);
	key_put(spnego_key);
out:
	sess_data->result = rc;
	sess_data->func = NULL;
	SMB2_sess_free_buffer(sess_data);
}