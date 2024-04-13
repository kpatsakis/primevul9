SMB2_select_sec(struct cifs_ses *ses, struct SMB2_sess_data *sess_data)
{
	int type;

	type = smb2_select_sectype(ses->server, ses->sectype);
	cifs_dbg(FYI, "sess setup type %d\n", type);
	if (type == Unspecified) {
		cifs_dbg(VFS,
			"Unable to select appropriate authentication method!");
		return -EINVAL;
	}

	switch (type) {
	case Kerberos:
		sess_data->func = SMB2_auth_kerberos;
		break;
	case RawNTLMSSP:
		sess_data->func = SMB2_sess_auth_rawntlmssp_negotiate;
		break;
	default:
		cifs_dbg(VFS, "secType %d not supported!\n", type);
		return -EOPNOTSUPP;
	}

	return 0;
}