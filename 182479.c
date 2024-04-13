SMB2_auth_kerberos(struct SMB2_sess_data *sess_data)
{
	cifs_dbg(VFS, "Kerberos negotiated but upcall support disabled!\n");
	sess_data->result = -EOPNOTSUPP;
	sess_data->func = NULL;
}