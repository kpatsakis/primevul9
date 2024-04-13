SMB2_get_srv_num(const unsigned int xid, struct cifs_tcon *tcon,
		 u64 persistent_fid, u64 volatile_fid, __le64 *uniqueid)
{
	return query_info(xid, tcon, persistent_fid, volatile_fid,
			  FILE_INTERNAL_INFORMATION, SMB2_O_INFO_FILE, 0,
			  sizeof(struct smb2_file_internal_info),
			  sizeof(struct smb2_file_internal_info),
			  (void **)&uniqueid, NULL);
}