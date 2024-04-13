SMB2_set_acl(const unsigned int xid, struct cifs_tcon *tcon,
		u64 persistent_fid, u64 volatile_fid,
		struct cifs_ntsd *pnntsd, int pacllen, int aclflag)
{
	return send_set_info(xid, tcon, persistent_fid, volatile_fid,
			current->tgid, 0, SMB2_O_INFO_SECURITY, aclflag,
			1, (void **)&pnntsd, &pacllen);
}