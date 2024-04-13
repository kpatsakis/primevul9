SMB2_set_ea(const unsigned int xid, struct cifs_tcon *tcon,
	    u64 persistent_fid, u64 volatile_fid,
	    struct smb2_file_full_ea_info *buf, int len)
{
	return send_set_info(xid, tcon, persistent_fid, volatile_fid,
		current->tgid, FILE_FULL_EA_INFORMATION, SMB2_O_INFO_FILE,
		0, 1, (void **)&buf, &len);
}