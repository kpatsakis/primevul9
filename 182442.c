SMB2_sess_establish_session(struct SMB2_sess_data *sess_data)
{
	int rc = 0;
	struct cifs_ses *ses = sess_data->ses;

	mutex_lock(&ses->server->srv_mutex);
	if (ses->server->ops->generate_signingkey) {
		rc = ses->server->ops->generate_signingkey(ses);
		if (rc) {
			cifs_dbg(FYI,
				"SMB3 session key generation failed\n");
			mutex_unlock(&ses->server->srv_mutex);
			return rc;
		}
	}
	if (!ses->server->session_estab) {
		ses->server->sequence_number = 0x2;
		ses->server->session_estab = true;
	}
	mutex_unlock(&ses->server->srv_mutex);

	cifs_dbg(FYI, "SMB2/3 session established successfully\n");
	spin_lock(&GlobalMid_Lock);
	ses->status = CifsGood;
	ses->need_reconnect = false;
	spin_unlock(&GlobalMid_Lock);
	return rc;
}