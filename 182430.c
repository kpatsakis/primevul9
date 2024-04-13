SMB2_close(const unsigned int xid, struct cifs_tcon *tcon,
	   u64 persistent_fid, u64 volatile_fid)
{
	return SMB2_close_flags(xid, tcon, persistent_fid, volatile_fid, 0);
}